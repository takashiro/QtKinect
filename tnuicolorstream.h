#ifndef TNUICOLORSTREAM_H
#define TNUICOLORSTREAM_H

#include "tnuistream.h"
#include "tnuiimagebuffer.h"

class TNuiColorStream : public TNuiStream
{
    Q_OBJECT

public:
    TNuiColorStream(INuiSensor *nuiSensor);
    ~TNuiColorStream();

public:
    // Start stream processing.
    virtual bool startStream();

    // Open stream with a certain image resolution.
    bool openStream();

    // Process a incoming stream frame
    virtual void processStreamFrame();

    // Set image type. Only color image types are acceptable
    void setImageType(NUI_IMAGE_TYPE type);

    // Set image resolution. Only 640x480 and 1280x960 formats are acceptable
    void setImageResolution(NUI_IMAGE_RESOLUTION resolution);

private:
    // Process the incoming color frame
    void processColor();

private:
    NUI_IMAGE_TYPE m_imageType;
    NUI_IMAGE_RESOLUTION m_imageResolution;
    TNuiImageBuffer m_imageBuffer;

signals:
    void newFrame(const TNuiImageBuffer *image);
    void imageTypeChanged(NUI_IMAGE_TYPE type);
};

#endif // TNUICOLORSTREAM_H
