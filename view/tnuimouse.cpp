#include "tnuimouse.h"

#include "tnuisensormanager.h"
#include "tnuisensor.h"
#include "tnuiskeletonstream.h"
#include "tnuiimagestream.h"
#include "tnuitracker.h"

#include <QSGSimpleRectNode>

TNuiMouse::TNuiMouse(QQuickItem *parent)
    : QQuickItem(parent)
{
    TNuiSensor *sensor = SensorManager->sensor();

    m_tracker = new TNuiTracker(sensor);
    connect(m_tracker, &TNuiTracker::moved, this, &TNuiMouse::setPosition);

    setFlags(flags() | QQuickItem::ItemHasContents);
}

TNuiMouse::~TNuiMouse()
{
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
