#ifndef TNUICOLORCAMERA_H
#define TNUICOLORCAMERA_H

#include "timage.h"

class TNuiStream;

class TNuiColorCamera : public TImage
{
public:
    TNuiColorCamera(QQuickItem *parent = 0);

protected:
    void tryOpenStream();
    void updateFrame();

    TNuiStream *m_stream;
};

#endif // TNUICOLORCAMERA_H
