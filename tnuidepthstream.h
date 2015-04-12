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

protected:
    INuiFrameTexture *readFrameTexture();
};

#endif // TNUIDEPTHSTREAM_H
