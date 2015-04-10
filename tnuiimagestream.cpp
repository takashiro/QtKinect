#include "tnuiimagestream.h"
#include "tnuisensor.h"

TNuiImageStream::TNuiImageStream(TNuiSensor *parent, Type imageType)
    : TNuiStream(parent)
    , m_streamHandle(INVALID_HANDLE_VALUE)
    , m_imageType(imageType)
    , m_dataSize(640 * 480 * 4)
    , m_imageResolution(Resolution_640x480)
    , m_flags(0)
{
    m_data = new uchar[m_dataSize];
}

TNuiImageStream::~TNuiImageStream()
{
    m_dataMutex.lock();
    delete[] m_data;
    m_dataMutex.unlock();
}

bool TNuiImageStream::open()
{
    INuiSensor *sensor = m_sensor->nativeSensor();
    m_isOpen = (S_OK == sensor->NuiImageStreamOpen(
                    (NUI_IMAGE_TYPE) m_imageType,
                    (NUI_IMAGE_RESOLUTION) m_imageResolution,
                    m_flags,
                    2,
                    m_frameReadyEvent,
                    &m_streamHandle));
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

    if (m_paused) {
        // Stream paused. Skip frame process and release the frame.
        goto ReleaseFrame;
    }

    INuiFrameTexture *texture = readFrameTexture();
    if (texture == nullptr)
        return false;

    // Lock the frame data so the Kinect knows not to modify it while we are reading it
    NUI_LOCKED_RECT lockedRect;
    texture->LockRect(0, &lockedRect, NULL, 0);

    // Make sure we've received valid data
    if (lockedRect.Pitch != 0) {
        m_dataMutex.lock();
        memcpy_s(m_data, m_dataSize, lockedRect.pBits, lockedRect.size);
        m_dataMutex.unlock();
        isValid = true;
    }

    // Unlock frame data
    texture->UnlockRect(0);

ReleaseFrame:
    m_frameMutex.lock();
    sensor->NuiImageStreamReleaseFrame(m_streamHandle, &m_frame);
    m_frameMutex.unlock();

    int maxi = 640 * 480 * 4;
    m_dataMutex.lock();
    for (int i = 3; i < maxi; i += 4)
        m_data[i] = 255;
    m_dataMutex.unlock();

    return isValid;
}

QImage TNuiColorStream::readImage()
{
    QImage image(640, 480, QImage::Format_RGBA8888);
    int k = 0;
    uint color;
    uchar *rgba = reinterpret_cast<uchar *>(&color);

    m_dataMutex.lock();
    for (int j = 0; j < 480; j++) {
        for (int i = 0; i < 640; i++) {
            rgba[0] = m_data[k++];
            rgba[1] = m_data[k++];
            rgba[2] = m_data[k++];
            rgba[3] = m_data[k++];
            image.setPixel(i, j, color);
        }
    }
    m_dataMutex.unlock();
    return image;
}

INuiFrameTexture *TNuiColorStream::readFrameTexture()
{
    return m_frame.pFrameTexture;
}

INuiFrameTexture *TNuiDepthStream::readFrameTexture()
{
    INuiFrameTexture *texture;
    BOOL nearMode = hasFlag(EnableNearMode) ? TRUE : FALSE;

    // Attempt to get the extended depth texture
    HRESULT hr = m_sensor->nativeSensor()->NuiImageFrameGetDepthImagePixelFrameTexture(m_streamHandle, &m_frame, &nearMode, &texture);
    if (FAILED(hr))
        return nullptr;

    return texture;
}
