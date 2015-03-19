#include "tnuimousearea.h"

#include "tnuisensormanager.h"
#include "tnuisensor.h"
#include "tnuiimagestream.h"
#include "tnuiskeletonstream.h"
#include "tnuitracker.h"

TNuiMouseArea::TNuiMouseArea(QQuickItem *parent)
    : QQuickItem(parent)
    , m_isFocused(false)
{
    TNuiSensor *sensor = SensorManager->sensor();

    m_colorStream = new TNuiColorStream(sensor);
    m_tracker = new TNuiTracker(sensor, m_colorStream);
    connect(m_tracker, &TNuiTracker::moved, this, &TNuiMouseArea::onMouseMoved);
}

TNuiMouseArea::~TNuiMouseArea()
{
}

void TNuiMouseArea::onMouseMoved(const QPointF &pos)
{
    QRectF rect = mapRectToScene(boundingRect());

    if (rect.contains(pos)) {
        if (m_isFocused) {
            QDateTime now = QDateTime::currentDateTime();
            if (m_focusTimeOffset.secsTo(now) >= 1) {
                m_focusTimeOffset = now;
                emit touched();
            }
        } else {
            m_isFocused = true;
            m_focusTimeOffset = QDateTime::currentDateTime();
            emit entered();
        }
    } else {
        if (m_isFocused) {
            m_isFocused = false;
            emit exited();
        }
    }
}
