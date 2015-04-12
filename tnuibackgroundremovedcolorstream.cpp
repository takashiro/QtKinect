#include "tnuibackgroundremovedcolorstream.h"
#include "tnuisensor.h"

class TNuiBackgroundRemovedColorStreamPrivate : public TNuiStream
{
    friend class TNuiBackgroundRemovedColorStream;

    TNuiBackgroundRemovedColorStreamPrivate(TNuiBackgroundRemovedColorStream *stream)
        : TNuiStream(stream->sensor())
        , stream(stream)
    {
        m_frameReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        NuiCreateBackgroundRemovedColorStream(m_sensor->nativeSensor(), &nativeStream);
    }

    ~TNuiBackgroundRemovedColorStreamPrivate()
    {
        stop();

        nativeStream->Release();
    }

    bool open()
    {
        NUI_IMAGE_RESOLUTION colorResolution = (NUI_IMAGE_RESOLUTION) stream->resolution();
        NUI_IMAGE_RESOLUTION depthResolution = (NUI_IMAGE_RESOLUTION) stream->m_depthStream->resolution();
        m_isOpen = (S_OK == nativeStream->Enable(colorResolution, depthResolution, m_frameReadyEvent));
        if (m_isOpen)
            start();
        return m_isOpen;
    }

    bool processNewFrame()
    {
        HRESULT hr;
        NUI_BACKGROUND_REMOVED_COLOR_FRAME frame;

        hr = nativeStream->GetNextFrame(0, &frame);
        if (FAILED(hr))
            return false;

        stream->m_dataMutex.lock();
        memcpy_s(stream->m_outputData, stream->m_dataSize, frame.pBackgroundRemovedColorData, frame.backgroundRemovedColorDataLength);
        stream->m_dataMutex.unlock();

        hr = nativeStream->ReleaseFrame(&frame);
        if (FAILED(hr))
            return false;

        return true;
    }

    TNuiBackgroundRemovedColorStream *stream;
    INuiBackgroundRemovedColorStream *nativeStream;
};

TNuiBackgroundRemovedColorStream::TNuiBackgroundRemovedColorStream(TNuiSensor *parent)
    : TNuiColorStream(parent)
    , m_trackedSkeleton(NUI_SKELETON_INVALID_TRACKING_ID)
    , p_ptr(new TNuiBackgroundRemovedColorStreamPrivate(this))
{
    m_inputData = m_colorData = new uchar[m_dataSize];

    connect(p_ptr, &TNuiBackgroundRemovedColorStreamPrivate::readyRead, this, &TNuiBackgroundRemovedColorStream::readyRead);

    m_depthStream = new TNuiDepthStream(parent, true);
    m_depthStream->setFlag(EnableNearMode, true);
    connect(m_depthStream, &TNuiDepthStream::readyRead, this, &TNuiBackgroundRemovedColorStream::processDepth);

    m_skeletonStream = new TNuiSkeletonStream(parent);
    connect(m_skeletonStream, &TNuiSkeletonStream::readyRead, this, &TNuiBackgroundRemovedColorStream::processSkeleton);
}

TNuiBackgroundRemovedColorStream::~TNuiBackgroundRemovedColorStream()
{
    stop();

    delete m_depthStream;
    delete m_skeletonStream;
    delete p_ptr;

    m_colorDataMutex.lock();
    delete[] m_colorData;
    m_colorDataMutex.unlock();
}

bool TNuiBackgroundRemovedColorStream::open()
{
    if (!TNuiColorStream::open()) {
        qDebug("Failed to open color stream");
        return false;
    }

    m_isOpen = false;

    if (!m_depthStream->open()) {
        qDebug("Failed to open depth stream");
        return false;
    }

    if (!m_skeletonStream->open()) {
        qDebug("Failed to open skeleton stream");
        return false;
    }

    if (!p_ptr->open()) {
        qDebug("Failed to open background removal stream");
        return false;
    }

    m_isOpen = true;
    return true;
}

bool TNuiBackgroundRemovedColorStream::close()
{
    return S_OK == p_ptr->nativeStream->Disable();
}

bool TNuiBackgroundRemovedColorStream::processNewFrame()
{
    bool isValid = TNuiColorStream::processNewFrame();
    if (isValid) {
        NUI_IMAGE_FRAME frame;
        readFrame(frame);
        m_colorDataMutex.lock();
        p_ptr->nativeStream->ProcessColor(m_dataSize, m_colorData, frame.liTimeStamp);
        m_colorDataMutex.unlock();
    }
    return false;
}

void TNuiBackgroundRemovedColorStream::processDepth()
{
    NUI_IMAGE_FRAME imageFrame;
    m_depthStream->readFrame(imageFrame);
    m_depthStream->lockData();
    p_ptr->nativeStream->ProcessDepth(m_depthStream->dataSize(), m_depthStream->data(), imageFrame.liTimeStamp);
    m_depthStream->unlockData();
}

void TNuiBackgroundRemovedColorStream::processSkeleton()
{
    NUI_SKELETON_FRAME frame;
    m_skeletonStream->readFrame(frame);

    NUI_SKELETON_DATA *skeletonData = frame.SkeletonData;
    // Background Removal Stream requires us to specifically tell it what skeleton ID to use as the foreground
    if (!chooseSkeleton(skeletonData))
        return;

    p_ptr->nativeStream->ProcessSkeleton(NUI_SKELETON_COUNT, skeletonData, frame.liTimeStamp);
}

bool TNuiBackgroundRemovedColorStream::chooseSkeleton(NUI_SKELETON_DATA *skeletonData)
{
    // First we go through the stream to find the closest skeleton, and also check whether our current tracked
    // skeleton is still visibile in the stream
    float closestSkeletonDistance = FLT_MAX;
    DWORD closestSkeleton = NUI_SKELETON_INVALID_TRACKING_ID;
    BOOL isTrackedSkeletonVisible = false;
    for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
        NUI_SKELETON_DATA skeleton = skeletonData[i];
        if (NUI_SKELETON_TRACKED == skeleton.eTrackingState) {
            if (m_trackedSkeleton == skeleton.dwTrackingID) {
                isTrackedSkeletonVisible = true;
                break;
            }

            if (skeleton.Position.z < closestSkeletonDistance) {
                closestSkeleton = skeleton.dwTrackingID;
                closestSkeletonDistance = skeleton.Position.z;
            }
        }
    }

    // Now we choose a new skeleton unless the currently tracked skeleton is still visible
    if (!isTrackedSkeletonVisible && closestSkeleton != NUI_SKELETON_INVALID_TRACKING_ID) {
        HRESULT hr = p_ptr->nativeStream->SetTrackedPlayer(closestSkeleton);
        if (FAILED(hr))
            return false;

        m_trackedSkeleton = closestSkeleton;
    }

    return true;
}
