#include "tnuitrackeritem.h"

#include "tnuisensormanager.h"
#include "tnuitracker.h"

TNuiTrackerItem::TNuiTrackerItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    TNuiSensor *sensor = SensorManager->sensor();

    m_tracker = new TNuiTracker(sensor);
    connect(m_tracker, &TNuiTracker::moved, this, &TNuiTrackerItem::setPosition);
}

TNuiTrackerItem::~TNuiTrackerItem()
{
}
