#ifndef TBACKGROUNDREMOVEDCAMERA_H
#define TBACKGROUNDREMOVEDCAMERA_H

#include "tvideo.h"

class TBackgroundRemover;

class TBackgroundRemovedCamera : public TVideo
{
public:
    TBackgroundRemovedCamera(QQuickItem *parent = 0);

protected:
    TBackgroundRemover *m_remover;
};

#endif // TBACKGROUNDREMOVEDCAMERA_H
