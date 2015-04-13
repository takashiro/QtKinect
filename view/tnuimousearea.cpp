#include "tnuimousearea.h"

#include "tnuisensormanager.h"
#include "tnuisensor.h"
#include "tnuiskeletonstream.h"
#include "tnuitracker.h"

#include <QTimer>

TNuiMouseArea::TNuiMouseArea(QQuickItem *parent)
    : QQuickItem(parent)
    , m_isUnderLeftHand(false)
    , m_isUnderRightHand(false)
{
    TNuiSensor *sensor = SensorManager->sensor();

    static TNuiTracker *leftTracker = new TNuiTracker(sensor, NUI_SKELETON_POSITION_HAND_LEFT);
    static TNuiTracker *rightTracker = new TNuiTracker(sensor, NUI_SKELETON_POSITION_HAND_RIGHT);
    connect(leftTracker, &TNuiTracker::moved, this, &TNuiMouseArea::onLeftHandMoved);
    connect(rightTracker, &TNuiTracker::moved, this, &TNuiMouseArea::onRightHandMoved);

    m_longTouchTimer = new QTimer(this);
    m_longTouchTimer->setSingleShot(true);
    m_longTouchTimer->setInterval(1500);
    connect(m_longTouchTimer, &QTimer::timeout, this, &TNuiMouseArea::checkLongTouch);
    connect(this, &TNuiMouseArea::entered, m_longTouchTimer, (void (QTimer::*)()) &QTimer::start);
    connect(this, &TNuiMouseArea::exited, m_longTouchTimer, &QTimer::stop);
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
        if (!isUnderHand) {
            isUnderHand = true;
            emit entered();
        }
    } else {
        if (isUnderHand) {
            isUnderHand = false;
            emit exited();
        }
    }
}

void TNuiMouseArea::checkLongTouch()
{
    if (m_isUnderLeftHand || m_isUnderRightHand)
        emit longTouched();
}
