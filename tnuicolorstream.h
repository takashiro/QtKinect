#ifndef TNUICOLORSTREAM_H
#define TNUICOLORSTREAM_H

#include "tnuistream.h"

class TNuiColorStream : public TNuiStream
{
public:
    TNuiColorStream(TNuiSensor *parent);

protected:
    void processNewFrame();
};

#endif // TNUICOLORSTREAM_H
