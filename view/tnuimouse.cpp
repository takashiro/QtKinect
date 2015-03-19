#include "tnuimouse.h"

#include "tnuisensormanager.h"
#include "tnuiskeletonstream.h"
#include "tnuiimagestream.h"
#include "tnuitracker.h"

#include <QSGSimpleRectNode>

TNuiMouse::TNuiMouse(QQuickItem *parent)
    : QQuickItem(parent)
{
    TNuiSensor *sensor = SensorManager->sensor();

    m_skeletonStream = new TNuiSkeletonStream(sensor, TNuiSkeletonStream::EnableInNearRange | TNuiSkeletonStream::EnableSeatedSupport);
    m_skeletonStream->open();

    m_colorStream = new TNuiColorStream(sensor);

    m_tracker = new TNuiTracker(m_skeletonStream, m_colorStream);
    connect(m_tracker, &TNuiTracker::rightHandMoved, this, &TNuiMouse::setPosition);

    setFlags(flags() | QQuickItem::ItemHasContents);
}

TNuiMouse::~TNuiMouse()
{
    m_skeletonStream->deleteLater();
    m_colorStream->deleteLater();
    m_tracker->deleteLater();
}

QSGNode *TNuiMouse::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    QSGSimpleRectNode *n = static_cast<QSGSimpleRectNode *>(node);
    if (!n) {
        n = new QSGSimpleRectNode;
        n->setColor(Qt::red);
    }
    n->setRect(boundingRect());
    return n;
}

class TNuiMouseAdder
{
public:
    TNuiMouseAdder()
    {
        qmlRegisterType<TNuiMouse>("Kinect", 1, 0, "TNuiMouse");
    }
};
TNuiMouseAdder adder;
