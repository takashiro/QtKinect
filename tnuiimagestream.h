#ifndef TNUIIMAGESTREAM_H
#define TNUIIMAGESTREAM_H

#include "tnuistream.h"

#include <QMutex>
#include <QImage>
#include <QMap>

class TNuiImageStream : public TNuiStream
{
    Q_OBJECT

public:
    friend class TNuiSensor;

    enum Type{
        DepthAndPlayerIndex = NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
        Color = NUI_IMAGE_TYPE_COLOR,
        ColorYUV = NUI_IMAGE_TYPE_COLOR_YUV,
        ColorRawYUV = NUI_IMAGE_TYPE_COLOR_RAW_YUV,
        Depth = NUI_IMAGE_TYPE_DEPTH,
        ColorInfrared = NUI_IMAGE_TYPE_COLOR_INFRARED,
        ColorRawBayer = NUI_IMAGE_TYPE_COLOR_RAW_BAYER
    };

    enum Resolution{
        InvalidResolution = NUI_IMAGE_RESOLUTION_INVALID,
        Resolution_80x60 = NUI_IMAGE_RESOLUTION_80x60,
        Resolution_320x240 = NUI_IMAGE_RESOLUTION_320x240,
        Resolution_640x480 = NUI_IMAGE_RESOLUTION_640x480,
        Resolution_1280x960 = NUI_IMAGE_RESOLUTION_1280x960
    };

    enum Flag{
        DepthMaximum = NUI_IMAGE_DEPTH_MAXIMUM,
        DepthMinimum = NUI_IMAGE_DEPTH_MINIMUM,
        DepthNoValue = NUI_IMAGE_DEPTH_NO_VALUE,
        DistinctOverflowDepthValues = NUI_IMAGE_STREAM_FLAG_DISTINCT_OVERFLOW_DEPTH_VALUES,
        EnableNearMode = NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE,
        SuppressNoFrameData = NUI_IMAGE_STREAM_FLAG_SUPPRESS_NO_FRAME_DATA,
        FrameLimitMaximum = NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM,
        PlayerIndexShift = NUI_IMAGE_PLAYER_INDEX_SHIFT,
        PlayerIndexMask = NUI_IMAGE_PLAYER_INDEX_MASK
    };
    typedef uint Flags;

    TNuiImageStream(TNuiSensor *parent, Type type);
    ~TNuiImageStream();

    bool open();
    Type type() const { return m_type; }

    void setResolution(Resolution resolution) { m_resolution = resolution; }
    Resolution resolution() const { return m_resolution; }

    void readFrame(NUI_IMAGE_FRAME &frame);
    HANDLE handle() const { return m_streamHandle; }

    void lockData() { m_dataMutex.lock(); }
    const uchar *data() const { return m_outputData; }
    void unlockData() { m_dataMutex.unlock(); }
    uint dataSize() const { return m_dataSize; }

    void setFlags(Flags flags) { m_flags = flags; }
    void setFlag(Flag flag, bool enabled);
    bool hasFlag(Flag flag) const { return (m_flags & flag) == flag; }

    void setFrameBufferSize(ulong size);
    ulong frameBufferSize() const { return m_frameBufferSize; }

protected:
    bool processNewFrame();

    virtual INuiFrameTexture *readFrameTexture() = 0;

    HANDLE m_streamHandle;
    uchar *m_inputData;
    uchar *m_outputData;
    uint m_dataSize;
    QMutex m_dataMutex;

    NUI_IMAGE_FRAME m_frame;
    QMutex m_frameMutex;

    Type m_type;
    Resolution m_resolution;
    Flags m_flags;
    ulong m_frameBufferSize;
    static QMap<HANDLE, HANDLE> m_frameReadyEvents;
};

class TNuiColorStream : public TNuiImageStream
{
    Q_OBJECT

public:
    TNuiColorStream(TNuiSensor *parent)
        : TNuiImageStream(parent, Color)
    {
    }

    QImage readImage();

protected:
    INuiFrameTexture *readFrameTexture();
};

class TNuiDepthStream : public TNuiImageStream
{
    Q_OBJECT

public:
    TNuiDepthStream(TNuiSensor *parent, bool enablePlayerIndex = false)
        : TNuiImageStream(parent, enablePlayerIndex ? DepthAndPlayerIndex : Depth)
    {
    }

protected:
    INuiFrameTexture *readFrameTexture();
};

#endif // TNUIIMAGESTREAM_H
