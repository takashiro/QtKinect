
#include "stdafx.h"
#include "tnuicolorstream.h"

TNuiColorStream::TNuiColorStream(INuiSensor* nuiSensor)
    : TNuiStream(nuiSensor)
    , m_imageType(NUI_IMAGE_TYPE_COLOR)
    , m_imageResolution(NUI_IMAGE_RESOLUTION_640x480)
{
}

TNuiColorStream::~TNuiColorStream()
{
}

bool TNuiColorStream::startStream()
{
    setImageType(NUI_IMAGE_TYPE_COLOR);                 // Set default image type to color image
    setImageResolution(NUI_IMAGE_RESOLUTION_640x480);   // Set default image resolution to 640x480
    return openStream();
}

bool TNuiColorStream::openStream()
{
    // Open color stream.
    HRESULT hr = m_nuiSensor->NuiImageStreamOpen(m_imageType, m_imageResolution, 0, 2, getFrameReadyEvent(), &m_streamHandle);

    // Reset image buffer
    if (SUCCEEDED(hr))
    {
        m_imageBuffer.setImageSize(m_imageResolution);  // Set source image resolution to image buffer
    }

    return hr == S_OK;
}

// Set image type. Only color image types are acceptable
void TNuiColorStream::setImageType(NUI_IMAGE_TYPE type)
{
    switch (type)
    {
    case NUI_IMAGE_TYPE_COLOR:
    case NUI_IMAGE_TYPE_COLOR_YUV:
    case NUI_IMAGE_TYPE_COLOR_INFRARED:
    case NUI_IMAGE_TYPE_COLOR_RAW_BAYER:
        m_imageType = type;
        break;

    default:
        break;
    }

    emit imageTypeChanged(m_imageType);
}

// Set image resolution. Only 640x480 and 1280x960 formats are acceptable
void TNuiColorStream::setImageResolution(NUI_IMAGE_RESOLUTION resolution)
{
    switch (resolution)
    {
    case NUI_IMAGE_RESOLUTION_640x480:
    case NUI_IMAGE_RESOLUTION_1280x960:
        m_imageResolution = resolution;
        break;

    default:
        break;
    }
}

// Process a incoming stream frame
void TNuiColorStream::processStreamFrame()
{
    if (WAIT_OBJECT_0 == WaitForSingleObject(getFrameReadyEvent(), 0))
    {
        // Frame ready event has been set. Proceed to process incoming frame
        processColor();
    }
}

// Process the incoming color frame
void TNuiColorStream::processColor()
{
    HRESULT hr;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the color frame
    hr = m_nuiSensor->NuiImageStreamGetNextFrame(m_streamHandle, 0, &imageFrame);
    if (FAILED(hr))
    {
        return;
    }

    if (m_paused)
    {
        // Stream paused. Skip frame process and release the frame.
        goto ReleaseFrame;
    }

    INuiFrameTexture* pTexture = imageFrame.pFrameTexture;

    // Lock the frame data so the Kinect knows not to modify it while we are reading it
    NUI_LOCKED_RECT lockedRect;
    pTexture->LockRect(0, &lockedRect, NULL, 0);

    // Make sure we've received valid data
    if (lockedRect.Pitch != 0)
    {
        switch (m_imageType)
        {
        case NUI_IMAGE_TYPE_COLOR_RAW_BAYER:    // Convert raw bayer data to color image and copy to image buffer
            m_imageBuffer.copyBayer(lockedRect.pBits, lockedRect.size);
            break;

        case NUI_IMAGE_TYPE_COLOR_INFRARED:     // Convert infrared data to color image and copy to image buffer
            m_imageBuffer.copyInfrared(lockedRect.pBits, lockedRect.size);
            break;

        default:    // Copy color data to image buffer
            m_imageBuffer.copyRGB(lockedRect.pBits, lockedRect.size);
            break;
        }

        emit newFrame(&m_imageBuffer);
    }

    // Unlock frame data
    pTexture->UnlockRect(0);

ReleaseFrame:
    m_nuiSensor->NuiImageStreamReleaseFrame(m_streamHandle, &imageFrame);
}
