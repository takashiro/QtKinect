#ifndef TNUIIMAGESTREAM_H
#define TNUIIMAGESTREAM_H

#include "tnuistream.h"

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

    bool open();
    QVector<uchar> data() const {return m_data;}

protected:
    bool processNewFrame();

    HANDLE m_streamHandle;
    QVector<uchar> m_data;

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
