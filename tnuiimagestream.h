#ifndef TNUIIMAGESTREAM_H
#define TNUIIMAGESTREAM_H

#include "tnuistream.h"

#include <QMutex>
#include <QImage>

class TNuiImageStream : public TNuiStream
{
public:
    friend class TNuiSensor;

    enum ImageType{
        DepthAndPlayerIndexType = NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
        ColorType = NUI_IMAGE_TYPE_COLOR,
        ColorYUVType = NUI_IMAGE_TYPE_COLOR_YUV,
        ColorRawYUVType = NUI_IMAGE_TYPE_COLOR_RAW_YUV,
        DepthType = NUI_IMAGE_TYPE_DEPTH,
        ColorInfraredType = NUI_IMAGE_TYPE_COLOR_INFRARED,
        ColorRawBayerType = NUI_IMAGE_TYPE_COLOR_RAW_BAYER
    };

    enum ImageResolution{
        InvalidResolution = NUI_IMAGE_RESOLUTION_INVALID,
        Resolution_80x60 = NUI_IMAGE_RESOLUTION_80x60,
        Resolution_320x240 = NUI_IMAGE_RESOLUTION_320x240,
        Resolution_640x480 = NUI_IMAGE_RESOLUTION_640x480,
        Resolution_1280x960 = NUI_IMAGE_RESOLUTION_1280x960
    };

    TNuiImageStream(TNuiSensor *parent, ImageType imageType);
    ~TNuiImageStream();

    bool open();
    ImageType imageType() const { return m_imageType; }
    ImageResolution imageResolution() const { return m_imageResolution; }
    const NUI_IMAGE_FRAME &frame() const { return m_frame; }
    const uchar *data() const { return m_data; }
    uint dataSize() const { return m_dataSize; }

protected:
    bool processNewFrame();

    HANDLE m_streamHandle;
    NUI_IMAGE_FRAME m_frame;
    uchar *m_data;
    uint m_dataSize;
    QMutex m_dataMutex;

    ImageType m_imageType;
    ImageResolution m_imageResolution;
};

class TNuiColorStream : public TNuiImageStream
{
public:
    TNuiColorStream(TNuiSensor *parent)
        : TNuiImageStream(parent, ColorType)
    {
    }

    QImage readImage();
};

class TNuiDepthStream : public TNuiImageStream
{
public:
    TNuiDepthStream(TNuiSensor *parent)
        : TNuiImageStream(parent, DepthType)
    {
    }
};

#endif // TNUIIMAGESTREAM_H
