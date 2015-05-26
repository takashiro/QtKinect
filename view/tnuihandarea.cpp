#include "tnuihandarea.h"

#include "tnuisensormanager.h"
#include "tnuisensor.h"
#include "tnuiskeletonstream.h"
#include "tnuitracker.h"

#include <QTimer>

TNuiTracker *TNuiHandArea::m_leftTracker;
TNuiTracker *TNuiHandArea::m_rightTracker;

TNuiHandArea::TNuiHandArea(QQuickItem *parent)
    : QQuickItem(parent)
    , m_isUnderLeftHand(false)
    , m_isUnderRightHand(false)
    , m_isPressed(false)
    , m_pressDownRange(0.3f)
    , m_pressUpRange(0.1f)
{
    TNuiSensor *sensor = SensorManager->sensor();

    static TNuiTracker *leftTracker = m_leftTracker = new TNuiTracker(sensor, NUI_SKELETON_POSITION_HAND_LEFT);
    connect(leftTracker, &TNuiTracker::moved, this, &TNuiHandArea::onLeftHandMoved);

    static TNuiTracker *rightTracker = m_rightTracker = new TNuiTracker(sensor, NUI_SKELETON_POSITION_HAND_RIGHT);
    connect(rightTracker, &TNuiTracker::moved, this, &TNuiHandArea::onRightHandMoved);

    connect(this, &TNuiHandArea::exited, this, &TNuiHandArea::resetState);

    m_longTouchTimer = new QTimer(this);
    m_longTouchTimer->setSingleShot(true);
    m_longTouchTimer->setInterval(1500);
    connect(m_longTouchTimer, &QTimer::timeout, this, &TNuiHandArea::longTouched);
    connect(this, &TNuiHandArea::entered, m_longTouchTimer, (void (QTimer::*)()) &QTimer::start);
    connect(this, &TNuiHandArea::exited, m_longTouchTimer, &QTimer::stop);

    m_longPressTimer = new QTimer(this);
    m_longPressTimer->setSingleShot(true);
    m_longPressTimer->setInterval(1500);
    connect(m_longPressTimer, &QTimer::timeout, this, &TNuiHandArea::longPressed);
    connect(this, &TNuiHandArea::pressDown, m_longPressTimer, (void (QTimer::*)()) &QTimer::start);
    connect(this, &TNuiHandArea::exited, m_longPressTimer, &QTimer::stop);
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
            if (m_isUnderLeftHand) {
                m_initialHandZ =  m_leftTracker->z();
                connect(m_leftTracker, &TNuiTracker::zChanged, this, &TNuiHandArea::onTrackerZChanged);
            } else {
                m_initialHandZ =  m_rightTracker->z();
                connect(m_rightTracker, &TNuiTracker::zChanged, this, &TNuiHandArea::onTrackerZChanged);
            }
            emit entered();
        }
    } else {
        if (isUnderHand) {
            disconnect(m_leftTracker, &TNuiTracker::zChanged, this, &TNuiHandArea::onTrackerZChanged);
            disconnect(m_rightTracker, &TNuiTracker::zChanged, this, &TNuiHandArea::onTrackerZChanged);
            isUnderHand = false;
            emit exited();
        }
    }
}

void TNuiHandArea::onTrackerZChanged(float z)
{
    m_handZ = z;
    emit handZChanged();
    if (m_isPressed) {
        if (m_initialHandZ - z < m_pressUpRange) {
            m_isPressed = false;
            emit pressUp();
        }
    } else {
        if (m_initialHandZ - z > m_pressDownRange) {
            m_isPressed = true;
            emit pressDown();
        }
    }
}

void TNuiHandArea::resetState()
{
    m_isPressed = false;
}
