#ifndef TNUICOLORSTREAM_H
#define TNUICOLORSTREAM_H

#include "tnuiimagestream.h"

#include <QPointer>
#include <QImage>

class TNuiColorStreamInternal : public TNuiImageStreamInternal
{
    Q_OBJECT

    friend class TNuiColorStream;

public:
    TNuiColorStreamInternal(TNuiSensor *sensor, QObject *parent = 0)
        : TNuiImageStreamInternal(sensor, parent)
    {
    }

protected:
    INuiFrameTexture *readFrameTexture();
};

class TNuiColorStream : public TNuiImageStream
{
    Q_OBJECT

public:
    TNuiColorStream(TNuiSensor *parent);

    QImage readImage();

protected:
    TNuiStreamInternal *createReader(TNuiSensor *sensor);
    static QPointer<TNuiColorStreamInternal> d;
};

#endif // TNUICOLORSTREAM_H
