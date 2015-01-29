#include "tbackgroundremovedcamera.h"
#include "tbackgroundremover.h"

TBackgroundRemovedCamera::TBackgroundRemovedCamera(QQuickItem *parent)
    : TVideo(parent)
{
    m_remover = new TBackgroundRemover(this);
    connect(m_remover, &TBackgroundRemover::newFrame, this, &TBackgroundRemovedCamera::setFrame);
}

class TBackgroundRemovedCameraAdder
{
public:
    TBackgroundRemovedCameraAdder()
    {
        qmlRegisterType<TBackgroundRemovedCamera>("Kinect", 1, 0, "TBackgroundRemovedCamera");
    }
};
TBackgroundRemovedCameraAdder adder;
