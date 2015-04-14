#ifndef TNUIDEPTHSTREAM_H
#define TNUIDEPTHSTREAM_H

#include "tnuiimagestream.h"

class TNuiDepthStream : public TNuiImageStream
{
    Q_OBJECT

public:
    TNuiDepthStream(TNuiSensor *parent, bool enablePlayerIndex = false)
        : TNuiImageStream(parent, enablePlayerIndex ? DepthAndPlayerIndex : Depth)
    {
    }

    bool inNearMode() const { return m_inNearMode; }

protected:
    INuiFrameTexture *readFrameTexture();

private:
    bool m_inNearMode;
};

#endif // TNUIDEPTHSTREAM_H
