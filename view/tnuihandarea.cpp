#include "tnuihandarea.h"

#include "tnuisensormanager.h"
#include "tnuisensor.h"
#include "tnuiskeletonstream.h"
#include "tnuitracker.h"

#include <QTimer>

TNuiHandArea::TNuiHandArea(QQuickItem *parent)
    : QQuickItem(parent)
    , m_isUnderLeftHand(false)
    , m_isUnderRightHand(false)
{
    TNuiSensor *sensor = SensorManager->sensor();

    static TNuiTracker *leftTracker = new TNuiTracker(sensor, NUI_SKELETON_POSITION_HAND_LEFT);
    static TNuiTracker *rightTracker = new TNuiTracker(sensor, NUI_SKELETON_POSITION_HAND_RIGHT);
    connect(leftTracker, &TNuiTracker::moved, this, &TNuiHandArea::onLeftHandMoved);
    connect(rightTracker, &TNuiTracker::moved, this, &TNuiHandArea::onRightHandMoved);

    m_longTouchTimer = new QTimer(this);
    m_longTouchTimer->setSingleShot(true);
    m_longTouchTimer->setInterval(1500);
    connect(m_longTouchTimer, &QTimer::timeout, this, &TNuiHandArea::checkLongTouch);
    connect(this, &TNuiHandArea::entered, m_longTouchTimer, (void (QTimer::*)()) &QTimer::start);
    connect(this, &TNuiHandArea::exited, m_longTouchTimer, &QTimer::stop);
}

TNuiHandArea::~TNuiHandArea()
{
}

void TNuiHandArea::onLeftHandMoved(const QPointF &pos)
{
    onHandMoved(m_isUnderLeftHand, pos);
}

void TNuiHandArea::onRightHandMoved(const QPointF &pos)
{
    onHandMoved(m_isUnderRightHand, pos);
}

void TNuiHandArea::onHandMoved(bool &isUnderHand, const QPointF &pos)
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

void TNuiHandArea::checkLongTouch()
{
    if (m_isUnderLeftHand || m_isUnderRightHand)
        emit longTouched();
}
