#include "tnuicolorstream.h"
#include "tnuisensor.h"

TNuiColorStream::TNuiColorStream(TNuiSensor *parent)
    : TNuiStream(parent)
{
    m_imageType = NUI_IMAGE_TYPE_COLOR;
    m_imageResolution = NUI_IMAGE_RESOLUTION_640x480;
}

void TNuiColorStream::processNewFrame()
{
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the color frame
    if (!m_sensor->readImageFrame(this, 0, imageFrame))
        return;

    if (m_paused) {
        // Stream paused. Skip frame process and release the frame.
        goto ReleaseFrame;
    }

    INuiFrameTexture *pTexture = imageFrame.pFrameTexture;

    // Lock the frame data so the Kinect knows not to modify it while we are reading it
    NUI_LOCKED_RECT lockedRect;
    pTexture->LockRect(0, &lockedRect, NULL, 0);

    // Make sure we've received valid data
    if (lockedRect.Pitch != 0) {
        m_data.resize(lockedRect.size);
        memcpy_s(m_data.data(), lockedRect.size, lockedRect.pBits, lockedRect.size);
    }

    // Unlock frame data
    pTexture->UnlockRect(0);

ReleaseFrame:
    m_sensor->releaseImageFrame(this, imageFrame);
}
