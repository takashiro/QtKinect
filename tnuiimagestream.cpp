#include "tnuiimagestream.h"
#include "tnuisensor.h"

TNuiImageStreamInternal::TNuiImageStreamInternal(TNuiSensor *sensor, QObject *parent)
    : TNuiStreamInternal(sensor, parent)
    , m_streamHandle(INVALID_HANDLE_VALUE)
    , m_dataSize(640 * 480 * 4)
    , m_resolution(TNuiImageStream::Resolution_640x480)
    , m_flags(0)
    , m_frameBufferSize(2)
{
    m_inputData = m_outputData = new uchar[m_dataSize];
}

TNuiImageStreamInternal::~TNuiImageStreamInternal()
{
    stop();

    m_dataMutex.lockForWrite();
    delete[] m_outputData;
    m_outputData = nullptr;
    m_dataMutex.unlock();
}

bool TNuiImageStreamInternal::open()
{
    INuiSensor *sensor = m_sensor->nativeSensor();
    m_frameReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    HRESULT hr = sensor->NuiImageStreamOpen(
                    (NUI_IMAGE_TYPE) m_type,
                    (NUI_IMAGE_RESOLUTION) m_resolution,
                    m_flags,
                    m_frameBufferSize,
                    m_frameReadyEvent,
                    &m_streamHandle);

    if (hr == S_OK) {
        m_isOpen = true;
        start();
    }

    return m_isOpen;
}


//@todo: Image stream can't be closed?
bool TNuiImageStreamInternal::close()
{
    return false;
}

bool TNuiImageStreamInternal::processNewFrame()
{
    bool isValid = false;

    INuiSensor *sensor = m_sensor->nativeSensor();

    // Attempt to get the color frame
    m_frameMutex.lockForWrite();
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
        m_dataMutex.lockForWrite();
        memcpy_s(m_inputData, m_dataSize, lockedRect.pBits, lockedRect.size);
        m_dataMutex.unlock();
        isValid = true;
    }

    // Unlock frame data
    texture->UnlockRect(0);

    m_frameMutex.lockForWrite();
    sensor->NuiImageStreamReleaseFrame(m_streamHandle, &m_frame);
    m_frameMutex.unlock();

    int maxi = 640 * 480 * 4;
    m_dataMutex.lockForWrite();
    for (int i = 3; i < maxi; i += 4)
        m_inputData[i] = 255;
    m_dataMutex.unlock();

    return isValid;
}

TNuiImageStream::TNuiImageStream(TNuiSensor *sensor)
    : TNuiStream(sensor)
{
}

TNuiImageStream::~TNuiImageStream()
{
}

void TNuiImageStream::setInternal(TNuiImageStreamInternal *internal)
{
    d = internal;
    TNuiStream::setInternal(internal);
}

TNuiImageStream::Type TNuiImageStream::type() const
{
    return d->m_type;
}

void TNuiImageStream::setResolution(Resolution resolution)
{
    d->m_resolution = resolution;
}

TNuiImageStream::Resolution TNuiImageStream::resolution() const
{
    return d->m_resolution;
}

void TNuiImageStream::readFrame(NUI_IMAGE_FRAME &frame)
{
    d->m_frameMutex.lockForRead();
    frame = d->m_frame;
    d->m_frameMutex.unlock();
}

HANDLE TNuiImageStream::handle() const
{
    return d->m_streamHandle;
}

void TNuiImageStream::lockData()
{
    d->m_dataMutex.lockForRead();
}

const uchar *TNuiImageStream::data() const
{
    return d->m_outputData;
}

void TNuiImageStream::unlockData()
{
    d->m_dataMutex.unlock();
}

uint TNuiImageStream::dataSize() const
{
    return d->m_dataSize;
}

void TNuiImageStream::setFlags(Flags flags)
{
    d->m_flags = flags;
}

void TNuiImageStream::setFlag(Flag flag, bool enabled)
{
    if (enabled) {
        d->m_flags |= flag;
    } else {
        d->m_flags &= ~flag;
    }
}

bool TNuiImageStream::hasFlag(Flag flag) const
{
    return (d->m_flags & flag) == flag;
}

void TNuiImageStream::setFrameBufferSize(ulong size)
{
    if (size <= NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM)
        d->m_frameBufferSize = size;
    else
        d->m_frameBufferSize = NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM;
}

ulong TNuiImageStream::frameBufferSize() const
{
    return d->m_frameBufferSize;
}
