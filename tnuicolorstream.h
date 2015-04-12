#ifndef TNUICOLORSTREAM_H
#define TNUICOLORSTREAM_H

#include "tnuiimagestream.h"

#include <QImage>

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

#endif // TNUICOLORSTREAM_H
