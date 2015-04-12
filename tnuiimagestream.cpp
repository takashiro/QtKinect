#include "tnuiimagestream.h"
#include "tnuisensor.h"

QMap<HANDLE, HANDLE> TNuiImageStream::m_frameReadyEvents;

TNuiImageStream::TNuiImageStream(TNuiSensor *parent, Type imageType)
    : TNuiStream(parent)
    , m_streamHandle(INVALID_HANDLE_VALUE)
    , m_type(imageType)
    , m_dataSize(640 * 480 * 4)
    , m_resolution(Resolution_640x480)
    , m_flags(0)
    , m_frameBufferSize(2)
{
    m_inputData = m_outputData = new uchar[m_dataSize];
}

TNuiImageStream::~TNuiImageStream()
{
    stop();

    m_dataMutex.lock();
    delete[] m_outputData;
    m_outputData = nullptr;
    m_dataMutex.unlock();
}

bool TNuiImageStream::open()
{
    INuiSensor *sensor = m_sensor->nativeSensor();
    m_frameReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_isOpen = (S_OK == sensor->NuiImageStreamOpen(
                    (NUI_IMAGE_TYPE) m_type,
                    (NUI_IMAGE_RESOLUTION) m_resolution,
                    m_flags,
                    m_frameBufferSize,
                    m_frameReadyEvent,
                    &m_streamHandle));

    //If the stream is already opened, the corresponding frame-ready event should be reused.
    //@to-do: Check what will happen if Kinect is restarted
    if (m_frameReadyEvents.contains(m_streamHandle)) {
        CloseHandle(m_frameReadyEvent);
        m_frameReadyEvent = m_frameReadyEvents.value(m_streamHandle);
    } else {
        m_frameReadyEvents[m_streamHandle] = m_frameReadyEvent;
    }

    if (m_isOpen)
        start();
    return m_isOpen;
}

void TNuiImageStream::readFrame(NUI_IMAGE_FRAME &frame)
{
    m_frameMutex.lock();
    frame = m_frame;
    m_frameMutex.unlock();
}

void TNuiImageStream::setFlag(Flag flag, bool enabled)
{
    if (enabled) {
        m_flags |= flag;
    } else {
        m_flags &= ~flag;
    }
}

void TNuiImageStream::setFrameBufferSize(ulong size)
{
    if (size <= NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM)
        m_frameBufferSize = size;
    else
        m_frameBufferSize = NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM;
}

bool TNuiImageStream::processNewFrame()
{
    bool isValid = false;

    INuiSensor *sensor = m_sensor->nativeSensor();

    // Attempt to get the color frame
    m_frameMutex.lock();
    if (S_OK != sensor->NuiImageStreamGetNextFrame(m_streamHandle, 0, &m_frame)) {
        m_frameMutex.unlock();
        return false;
    }
    m_frameMutex.unlock();

    INuiFrameTexture *texture = readFrameTexture();
    if (texture == nullptr)
        return false;

    // Lock the frame data so the Kinect knows not to modify it while we are reading it
    NUI_LOCKED_RECT lockedRect;
    texture->LockRect(0, &lockedRect, NULL, 0);

    // Make sure we've received valid data
    if (lockedRect.Pitch != 0) {
        m_dataMutex.lock();
        memcpy_s(m_inputData, m_dataSize, lockedRect.pBits, lockedRect.size);
        m_dataMutex.unlock();
        isValid = true;
    }

    // Unlock frame data
    texture->UnlockRect(0);

    m_frameMutex.lock();
    sensor->NuiImageStreamReleaseFrame(m_streamHandle, &m_frame);
    m_frameMutex.unlock();

    int maxi = 640 * 480 * 4;
    m_dataMutex.lock();
    for (int i = 3; i < maxi; i += 4)
        m_inputData[i] = 255;
    m_dataMutex.unlock();

    return isValid;
}
