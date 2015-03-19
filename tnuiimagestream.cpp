#include "tnuiimagestream.h"
#include "tnuisensor.h"

TNuiImageStream::TNuiImageStream(TNuiSensor *parent, ImageType imageType)
    : TNuiStream(parent)
    , m_streamHandle(INVALID_HANDLE_VALUE)
    , m_imageType(imageType)
    , m_imageResolution(Resolution_640x480)
{
}

bool TNuiImageStream::open()
{
    m_isOpen = m_sensor->_openImageStream(this, 0, 2);
    if (m_isOpen)
        start();
    return m_isOpen;
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
        m_data.resize(lockedRect.size);
        memcpy_s(m_data.data(), lockedRect.size, lockedRect.pBits, lockedRect.size);
        isValid = true;
    }

    // Unlock frame data
    pTexture->UnlockRect(0);

ReleaseFrame:
    m_sensor->_releaseImageFrame(this, m_frame);
    return isValid;
}
