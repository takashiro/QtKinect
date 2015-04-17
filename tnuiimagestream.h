#ifndef TNUIIMAGESTREAM_H
#define TNUIIMAGESTREAM_H

#include "tnuistream.h"

#include <QReadWriteLock>
#include <QMap>

class TNuiImageStreamInternal;

class TNuiImageStream : public TNuiStream
{
    Q_OBJECT

public:
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

    TNuiImageStream(TNuiSensor *sensor);
    ~TNuiImageStream();

    Type type() const;

    void setResolution(Resolution resolution);
    Resolution resolution() const;
    void readFrame(NUI_IMAGE_FRAME &frame);
    HANDLE handle() const;
    void lockData();
    const uchar *data() const;
    void unlockData();
    uint dataSize() const;

    void setFlags(Flags flags);
    void setFlag(Flag flag, bool enabled);
    bool hasFlag(Flag flag) const;

    void setFrameBufferSize(ulong size);
    ulong frameBufferSize() const;

protected:
    void setInternal(TNuiImageStreamInternal *internal);

private:
    TNuiImageStreamInternal *d;
};

class TNuiImageStreamInternal : public TNuiStreamInternal
{
    Q_OBJECT

    friend class TNuiImageStream;

public:
    TNuiImageStreamInternal(TNuiSensor *sensor, QObject *parent = 0);
    ~TNuiImageStreamInternal();

    bool open();
    bool close();

protected:
    bool processNewFrame();
    virtual INuiFrameTexture *readFrameTexture() = 0;

    HANDLE m_streamHandle;
    uchar *m_inputData;
    uchar *m_outputData;
    uint m_dataSize;
    QReadWriteLock m_dataMutex;

    NUI_IMAGE_FRAME m_frame;
    QReadWriteLock m_frameMutex;

    TNuiImageStream::Type m_type;
    TNuiImageStream::Resolution m_resolution;
    TNuiImageStream::Flags m_flags;
    ulong m_frameBufferSize;
};

#endif // TNUIIMAGESTREAM_H
