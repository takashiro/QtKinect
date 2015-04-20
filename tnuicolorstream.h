#ifndef TNUICOLORSTREAM_H
#define TNUICOLORSTREAM_H

#include "tnuiimagestream.h"

#include <QPointer>
#include <QImage>
#include <QSet>

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

class TNuiColorStreamEffect;

class TNuiColorStream : public TNuiImageStream
{
    Q_OBJECT

public:
    TNuiColorStream(TNuiSensor *parent);
    ~TNuiColorStream();

    void addEffect(TNuiColorStreamEffect *effect);
    void removeEffect(TNuiColorStreamEffect *effect);
    bool hasEffect(TNuiColorStreamEffect *effect);
    QImage readImage();

protected:
    QSet<TNuiColorStreamEffect *> m_effects;
    QMutex m_effectMutex;
    uchar *m_image;
    uint m_imageSize;
    static QPointer<TNuiColorStreamInternal> d;
};

#endif // TNUICOLORSTREAM_H
