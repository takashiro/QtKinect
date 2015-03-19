#include "tnuimousearea.h"

#include "tnuisensormanager.h"
#include "tnuisensor.h"
#include "tnuiimagestream.h"
#include "tnuiskeletonstream.h"
#include "tnuitracker.h"

TNuiMouseArea::TNuiMouseArea(QQuickItem *parent)
    : QQuickItem(parent)
    , m_isUnderLeftHand(false)
    , m_isUnderRightHand(false)
{
    TNuiSensor *sensor = SensorManager->sensor();

    TNuiColorStream *colorStream = new TNuiColorStream(sensor);
    TNuiTracker *leftTracker = new TNuiTracker(sensor, colorStream, NUI_SKELETON_POSITION_HAND_LEFT);
    TNuiTracker *rightTracker = new TNuiTracker(sensor, colorStream, NUI_SKELETON_POSITION_HAND_RIGHT);
    connect(leftTracker, &TNuiTracker::moved, this, &TNuiMouseArea::onLeftHandMoved);
    connect(rightTracker, &TNuiTracker::moved, this, &TNuiMouseArea::onRightHandMoved);
}

TNuiMouseArea::~TNuiMouseArea()
{
}

void TNuiMouseArea::onLeftHandMoved(const QPointF &pos)
{
    onHandMoved(m_isUnderLeftHand, pos);
}

void TNuiMouseArea::onRightHandMoved(const QPointF &pos)
{
    onHandMoved(m_isUnderRightHand, pos);
}

void TNuiMouseArea::onHandMoved(bool &isUnderHand, const QPointF &pos)
{
    QRectF rect = mapRectToScene(boundingRect());

    if (rect.contains(pos)) {
        if (isUnderHand) {
            QDateTime now = QDateTime::currentDateTime();
            if (m_focusTimeOffset.secsTo(now) >= 1) {
                m_focusTimeOffset = now;
                emit touched();
            }
        } else {
            isUnderHand = true;
            m_focusTimeOffset = QDateTime::currentDateTime();
            emit entered();
        }
    } else {
        if (isUnderHand) {
            isUnderHand = false;
            emit exited();
        }
    }
}
