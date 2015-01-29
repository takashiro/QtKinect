#include "tbackgroundremover.h"
#include "tnuisensorselector.h"

#include <stdafx.h>

#include <QImage>

#define WM_SENSORCHANGED WM_USER + 1

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

TBackgroundRemover::TBackgroundRemover(QObject *parent) :
    QThread(parent),
    m_depthStreamHandle(INVALID_HANDLE_VALUE),
    m_colorStreamHandle(INVALID_HANDLE_VALUE),
    m_nearMode(false),
    m_nuiSensor(nullptr),
    m_sensorSelector(new TNuiSensorSelector),
    m_backgroundRemovalStream(nullptr),
    m_trackedSkeleton(NUI_SKELETON_INVALID_TRACKING_ID)
{
    ulong width = 0;
    ulong height = 0;

    NuiImageResolutionToSize(DEPTH_RESOLUTION, width, height);
    m_depthWidth  = static_cast<LONG>(width);
    m_depthHeight = static_cast<LONG>(height);

    NuiImageResolutionToSize(COLOR_RESOLUTION, width, height);
    m_colorWidth  = static_cast<LONG>(width);
    m_colorHeight = static_cast<LONG>(height);;

    // create heap storage for depth pixel data in RGBX format
    m_outputRGBX = new uchar[m_colorWidth * m_colorHeight * BYTES_PER_PIXEL];
    m_backgroundRGBX = new uchar[m_colorWidth * m_colorHeight * BYTES_PER_PIXEL];

    // Create an event that will be signaled when depth data is available
    m_nextDepthFrameEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    // Create an event that will be signaled when color data is available
    m_nextColorFrameEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    // Create an event that will be signaled when skeleton frame is available
    m_nextSkeletonFrameEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    // Create an event that will be signaled when the segmentation frame is ready
    m_nextBackgroundRemovedFrameEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);


    // Set the sensor status callback
    NuiSetDeviceStatusCallback(StatusChangeCallback, this);

    // Look for a connected Kinect, and create it if found
    if(!createFirstConnected())
        emit toastMessage("No connected Kinect was found.");

    // Create the stream that does background removal and player segmentation
    if(!createBackgroundRemovedColorStream())
        emit toastMessage("Create background removal stream failed.");

    setBackground("ui/image/default-background.jpg");
    start();
}

TBackgroundRemover::~TBackgroundRemover()
{
    // clean up arrays
    delete[] m_outputRGBX;
    delete[] m_backgroundRGBX;

    // clean up NSC sensor selector
    delete m_sensorSelector;

    SafeCloseHandle(m_nextBackgroundRemovedFrameEvent);
    SafeCloseHandle(m_nextColorFrameEvent);
    SafeCloseHandle(m_nextDepthFrameEvent);
    SafeCloseHandle(m_nextSkeletonFrameEvent);

    SafeRelease(m_nuiSensor);
    SafeRelease(m_backgroundRemovalStream);
}

void TBackgroundRemover::setBackground(const QString &filePath)
{
    QImage image(filePath);
    image = image.convertToFormat(QImage::Format_RGBA8888);

    int index = 0;
    for (int i = 0; i < 640; i++) {
        for (int j = 0; j < 480; j++) {
            QRgb color = image.pixel(i, j);
            m_backgroundRGBX[index + 3] = color & 0xFF;
            color >>= 8;
            m_backgroundRGBX[index] = color & 0xFF;
            color >>= 8;
            m_backgroundRGBX[index + 1] = color & 0xFF;
            color >>= 8;
            m_backgroundRGBX[index + 2] = color & 0xFF;
        }
    }
}

// Create the first connected Kinect found
bool TBackgroundRemover::createFirstConnected()
{
    HRESULT hr;

    // Get the Kinect and specify that we'll be using depth
    m_nuiSensor = m_sensorSelector->getSensor(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_COLOR);

    if (nullptr != m_nuiSensor) {
        // Open a depth image stream to receive depth frames
        hr = m_nuiSensor->NuiImageStreamOpen(
            NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
            DEPTH_RESOLUTION,
            0,
            2,
            m_nextDepthFrameEvent,
            &m_depthStreamHandle);
        if (SUCCEEDED(hr)) {
            // Open a color image stream to receive color frames
            hr = m_nuiSensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_COLOR,
                COLOR_RESOLUTION,
                0,
                2,
                m_nextColorFrameEvent,
                &m_colorStreamHandle);

            if (SUCCEEDED(hr)) {
                hr = m_nuiSensor->NuiSkeletonTrackingEnable(m_nextSkeletonFrameEvent, NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE);
            }
        }
    }

    if (nullptr == m_nuiSensor || FAILED(hr)) {
        SafeRelease(m_nuiSensor);
        // Reset all the event to nonsignaled state
        ResetEvent(m_nextDepthFrameEvent);
        ResetEvent(m_nextColorFrameEvent);
        ResetEvent(m_nextSkeletonFrameEvent);
        emit toastMessage("No ready Kinect found!");
        return false;
    } else {
        emit toastMessage("Kinect found!");
    }

    return hr == S_OK;
}

// Create the stream that removes the background and display a single player specified
bool TBackgroundRemover::createBackgroundRemovedColorStream()
{

    if (nullptr == m_nuiSensor) {
        return false;
    }

    HRESULT hr = NuiCreateBackgroundRemovedColorStream(m_nuiSensor, &m_backgroundRemovalStream);
    if (FAILED(hr))
    {
        return false;
    }

    hr = m_backgroundRemovalStream->Enable(COLOR_RESOLUTION, DEPTH_RESOLUTION, m_nextBackgroundRemovedFrameEvent);

    return !FAILED(hr);
}

// Handle new depth data
HRESULT TBackgroundRemover::processDepth()
{
    HRESULT hr;
    HRESULT bghr = S_OK;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the depth frame
    LARGE_INTEGER depthTimeStamp;
    hr = m_nuiSensor->NuiImageStreamGetNextFrame(m_depthStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
    {
        return false;
    }
    depthTimeStamp = imageFrame.liTimeStamp;
    INuiFrameTexture* texture;

    // Attempt to get the extended depth texture
    BOOL nearMode = m_nearMode ? TRUE : FALSE;
    hr = m_nuiSensor->NuiImageFrameGetDepthImagePixelFrameTexture(m_depthStreamHandle, &imageFrame, &nearMode, &texture);
    m_nearMode = (nearMode == TRUE);
    if (FAILED(hr))
    {
        return false;
    }
    NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    texture->LockRect(0, &LockedRect, nullptr, 0);

    // Make sure we've received valid data, and then present it to the background removed color stream.
    if (LockedRect.Pitch != 0)
    {
        bghr = m_backgroundRemovalStream->ProcessDepth(m_depthWidth * m_depthHeight * BYTES_PER_PIXEL, LockedRect.pBits, depthTimeStamp);
    }

    // We're done with the texture so unlock it. Even if above process failed, we still need to unlock and release.
    texture->UnlockRect(0);
    texture->Release();

    // Release the frame
    hr = m_nuiSensor->NuiImageStreamReleaseFrame(m_depthStreamHandle, &imageFrame);

    if (FAILED(bghr))
    {
        return false;
    }

    //
    return hr == S_OK;
}

/// <summary>
/// Handle new color data
/// </summary>
/// <returns>S_OK for success or error code</returns>
bool TBackgroundRemover::processColor()
{
    HRESULT hr;
    HRESULT bghr = S_OK;

    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the depth frame
    LARGE_INTEGER colorTimeStamp;
    hr = m_nuiSensor->NuiImageStreamGetNextFrame(m_colorStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
    {
        return false;
    }
    colorTimeStamp = imageFrame.liTimeStamp;

    INuiFrameTexture *texture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    texture->LockRect(0, &LockedRect, nullptr, 0);

    // Make sure we've received valid data. Then save a copy of color frame.
    if (LockedRect.Pitch != 0) {
        bghr = m_backgroundRemovalStream->ProcessColor(m_colorWidth * m_colorHeight * BYTES_PER_PIXEL, LockedRect.pBits, colorTimeStamp);
    }

    // We're done with the texture so unlock it
    texture->UnlockRect(0);

    // Release the frame
    hr = m_nuiSensor->NuiImageStreamReleaseFrame(m_colorStreamHandle, &imageFrame);

    if (FAILED(bghr))
    {
        return false;
    }

    return hr == S_OK;
}

bool TBackgroundRemover::processSkeleton()
{
    HRESULT hr;

    NUI_SKELETON_FRAME skeletonFrame;
    hr = m_nuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if (FAILED(hr))
    {
        return false;
    }

    NUI_SKELETON_DATA* pSkeletonData = skeletonFrame.SkeletonData;
    // Background Removal Stream requires us to specifically tell it what skeleton ID to use as the foreground
    hr = chooseSkeleton(pSkeletonData);
    if (FAILED(hr))
    {
        return false;
    }

    hr = m_backgroundRemovalStream->ProcessSkeleton(NUI_SKELETON_COUNT, pSkeletonData, skeletonFrame.liTimeStamp);

    return hr == S_OK;
}

// compose the background removed color image with the background image
bool TBackgroundRemover::composeImage()
{
    HRESULT hr;
    NUI_BACKGROUND_REMOVED_COLOR_FRAME bgRemovedFrame;

    hr = m_backgroundRemovalStream->GetNextFrame(0, &bgRemovedFrame);
    if (FAILED(hr))
    {
        return false;
    }

    const uchar *backgroundRemovedColor = bgRemovedFrame.pBackgroundRemovedColorData;

    int dataLength = static_cast<int>(m_colorWidth) * static_cast<int>(m_colorHeight) * BYTES_PER_PIXEL;
    uchar alpha = 0;
    const int alphaChannelBytePosition = 3;
    for (int i = 0; i < dataLength; i++) {
        if (i % BYTES_PER_PIXEL == 0) {
            alpha = backgroundRemovedColor[i + alphaChannelBytePosition];
        }

        if (i % BYTES_PER_PIXEL != alphaChannelBytePosition) {
            m_outputRGBX[i] = static_cast<uchar>(((UCHAR_MAX - alpha) * m_backgroundRGBX[i] + alpha * backgroundRemovedColor[i]) / UCHAR_MAX);
        }
    }

    hr = m_backgroundRemovalStream->ReleaseFrame(&bgRemovedFrame);
    if (FAILED(hr))
    {
        return false;
    }

    //@todo: Well, isn't it RGBX? I see nothing but black.
    QImage frame(m_colorWidth, m_colorHeight, QImage::Format_RGBX8888);
    uchar *pixel = m_outputRGBX;
    for (uint i = 0; i < m_colorWidth; i++) {
        for (uint j = 0; j < m_colorHeight; j++) {
            uint rgb = 0;
            for (int k = 0; k < BYTES_PER_PIXEL; k++) {
                rgb = *pixel;
                rgb <<= 8;
                pixel++;
            }
            frame.setPixel(i, j, rgb);
        }
    }
    emit newFrame(frame);

    return true;
}

// Use the sticky player logic to determine the player whom the background removed
// color stream should consider as foreground.
HRESULT TBackgroundRemover::chooseSkeleton(NUI_SKELETON_DATA* pSkeletonData)
{
    HRESULT hr = S_OK;

    // First we go through the stream to find the closest skeleton, and also check whether our current tracked
    // skeleton is still visibile in the stream
    float closestSkeletonDistance = FLT_MAX;
    ulong closestSkeleton = NUI_SKELETON_INVALID_TRACKING_ID;
    BOOL isTrackedSkeletonVisible = false;
    for (int i = 0; i < NUI_SKELETON_COUNT; ++i)
    {
        NUI_SKELETON_DATA skeleton = pSkeletonData[i];
        if (NUI_SKELETON_TRACKED == skeleton.eTrackingState)
        {
            if (m_trackedSkeleton == skeleton.dwTrackingID)
            {
                isTrackedSkeletonVisible = true;
                break;
            }

            if (skeleton.Position.z < closestSkeletonDistance)
            {
                closestSkeleton = skeleton.dwTrackingID;
                closestSkeletonDistance = skeleton.Position.z;
            }
        }
    }

    // Now we choose a new skeleton unless the currently tracked skeleton is still visible
    if (!isTrackedSkeletonVisible && closestSkeleton != NUI_SKELETON_INVALID_TRACKING_ID)
    {
        hr = m_backgroundRemovalStream->SetTrackedPlayer(closestSkeleton);
        if (FAILED(hr))
        {
            return hr;
        }

        m_trackedSkeleton = closestSkeleton;
    }

    return hr;
}

// Update the sensor and status based on the input changeg flags
void TBackgroundRemover::updateSensorAndStatus(TNuiSensorSelector::ChangeFlag changeFlag)
{
    switch(changeFlag)
    {
    case TNuiSensorSelector::SensorChangedFlag:
        {
            // Free the previous sensor and try to get a new one
            SafeRelease(m_nuiSensor);
            HRESULT hr = createFirstConnected();
            if (SUCCEEDED(hr))
            {
                createBackgroundRemovedColorStream();
            }
        }

    case TNuiSensorSelector::StatusChangedFlag:
        emit statusChanged(m_sensorSelector->getStatus());
        break;
    }
}

void TBackgroundRemover::StatusChangeCallback(HRESULT, const OLECHAR *, const OLECHAR *, void *userData)
{
    TBackgroundRemover *remover = reinterpret_cast<TBackgroundRemover *>(userData);
    if (remover != nullptr && remover->m_sensorSelector != nullptr)
    {
        // Handle sensor status change event
        TNuiSensorSelector::ChangeFlag flag = remover->m_sensorSelector->onNuiStatusChanged();
        remover->updateSensorAndStatus(flag);
    }
}

void TBackgroundRemover::run()
{
    const HANDLE hEvents[] = {m_nextDepthFrameEvent, m_nextColorFrameEvent, m_nextSkeletonFrameEvent, m_nextBackgroundRemovedFrameEvent};
    MSG msg = {0};

    // Main message loop
    while (WM_QUIT != msg.message)
    {
        // Check to see if we have either a message (by passing in QS_ALLINPUT)
        // Or a Kinect event (hEvents)
        // Update() will check for Kinect events individually, in case more than one are signaled
        MsgWaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, INFINITE, QS_ALLINPUT);

        // Individually check the Kinect stream events since MsgWaitForMultipleObjects
        // can return for other reasons even though these are signaled.
        if (nullptr == m_nuiSensor) {
            return;
        }

        if (WAIT_OBJECT_0 == WaitForSingleObject(m_nextBackgroundRemovedFrameEvent, 0)) {
            composeImage();
        }

        if ( WAIT_OBJECT_0 == WaitForSingleObject(m_nextDepthFrameEvent, 0) ) {
            processDepth();
        }

        if ( WAIT_OBJECT_0 == WaitForSingleObject(m_nextColorFrameEvent, 0) ) {
            processColor();
        }

        if (WAIT_OBJECT_0 == WaitForSingleObject(m_nextSkeletonFrameEvent, 0) ) {
            processSkeleton();
        }

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}
