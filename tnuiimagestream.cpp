#include "tnuiimagestream.h"
#include "tnuisensor.h"

TNuiImageStream::TNuiImageStream(TNuiSensor *parent, ImageType imageType)
    : TNuiStream(parent)
    , m_streamHandle(INVALID_HANDLE_VALUE)
    , m_imageType(imageType)
    , m_image(640, 480, QImage::Format_RGB32)
    , m_imageResolution(Resolution_640x480)
{
}

TNuiImageStream::~TNuiImageStream()
{
    m_imageMutex.lock();
    m_imageMutex.unlock();
}

bool TNuiImageStream::open()
{
    m_isOpen = m_sensor->_openImageStream(this, 0, 2);
    if (m_isOpen)
        start();
    return m_isOpen;
}

QImage TNuiImageStream::readFrameImage()
{
    m_imageMutex.lock();
    QImage image = m_image;
    m_imageMutex.unlock();
    return image;
}

bool TNuiImageStream::processNewFrame()
{
    bool isValid = false;

    // Attempt to get the color frame
    if (!m_sensor->_readImageFrame(this, 0, m_frame))
        return false;

    if (m_paused) {
        // Stream paused. Skip frame process and release the frame.
        goto ReleaseFrame;
    }

    INuiFrameTexture *pTexture = m_frame.pFrameTexture;

    // Lock the frame data so the Kinect knows not to modify it while we are reading it
    NUI_LOCKED_RECT lockedRect;
    pTexture->LockRect(0, &lockedRect, NULL, 0);

    // Make sure we've received valid data
    if (lockedRect.Pitch != 0) {
        m_imageMutex.lock();
        int k = 0;
        uint color;
        uchar *rgba = reinterpret_cast<uchar *>(&color);
        for (int j = 0; j < 480; j++) {
            for (int i = 0; i < 640; i++) {
                rgba[0] = lockedRect.pBits[k++];
                rgba[1] = lockedRect.pBits[k++];
                rgba[2] = lockedRect.pBits[k++];
                rgba[3] = lockedRect.pBits[k++];
                m_image.setPixel(i, j, color);
            }
        }
        m_imageMutex.unlock();
        isValid = true;
    }

    // Unlock frame data
    pTexture->UnlockRect(0);

ReleaseFrame:
    m_sensor->_releaseImageFrame(this, m_frame);
    return isValid;
}
