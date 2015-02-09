#ifndef TNUICOLORCAMERA_H
#define TNUICOLORCAMERA_H

#include "timage.h"

class TNuiImageStream;

class TNuiColorCamera : public TImage
{
public:
    TNuiColorCamera(QQuickItem *parent = 0);

protected:
    void tryOpenStream();
    void updateFrame();

    TNuiImageStream *m_stream;
};

#endif // TNUICOLORCAMERA_H
