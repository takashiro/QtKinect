#ifndef TBACKGROUNDREMOVEDCAMERA_H
#define TBACKGROUNDREMOVEDCAMERA_H

#include "timage.h"

class TBackgroundRemover;

class TBackgroundRemovedCamera : public TImage
{
public:
    TBackgroundRemovedCamera(QQuickItem *parent = 0);

protected:
    TBackgroundRemover *m_remover;
};

#endif // TBACKGROUNDREMOVEDCAMERA_H
