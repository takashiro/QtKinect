#ifndef TNUIDEPTHSTREAM_H
#define TNUIDEPTHSTREAM_H

#include "tnuiimagestream.h"

#include <QPointer>

class TNuiDepthStreamInternal : public TNuiImageStreamInternal
{
    Q_OBJECT

    friend class TNuiDepthStream;

public:
    TNuiDepthStreamInternal(TNuiSensor *sensor, QObject *parent = 0);

protected:
    INuiFrameTexture *readFrameTexture();

    bool m_inNearMode;
};

class TNuiDepthStream : public TNuiImageStream
{
    Q_OBJECT

public:
    TNuiDepthStream(TNuiSensor *sensor, bool enablePlayerIndex = false);

    bool inNearMode() const { return d->m_inNearMode; }

    static QPointer<TNuiDepthStreamInternal> d;
};

#endif // TNUIDEPTHSTREAM_H
