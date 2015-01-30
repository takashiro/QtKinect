#ifndef TNUICOLORCAMERA_H
#define TNUICOLORCAMERA_H

#include "tnuiimage.h"

class TNuiColorStream;

class TNuiColorCamera : public TNuiImage
{
public:
    TNuiColorCamera(QQuickItem *parent = 0);

protected:
    TNuiColorStream *m_stream;
};

#endif // TNUICOLORCAMERA_H
