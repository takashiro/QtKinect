#include "tnuitrackeritem.h"

#include "tnuisensormanager.h"
#include "tnuitracker.h"

const char *TargetEnum[] = {
    "hip_center",
    "spine",
    "shoulder_center",
    "head",
    "shoulder_left",
    "elbow_left",
    "wrist_left",
    "hand_left",
    "shoulder_right",
    "elbow_right",
    "wrist_right",
    "hand_right",
    "hip_left",
    "knee_left",
    "ankle_left",
    "foot_left",
    "hip_right",
    "knee_right",
    "ankle_right",
    "foot_right"
};

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

QString TNuiTrackerItem::target() const
{
    NUI_SKELETON_POSITION_INDEX index = m_tracker->target();
    if (index >= 0 && index <= NUI_SKELETON_POSITION_COUNT)
        return TargetEnum[index];
    return QString();
}

void TNuiTrackerItem::setTarget(const QString &target)
{
    for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++) {
        if (target == TargetEnum[i]) {
            m_tracker->setTarget(NUI_SKELETON_POSITION_INDEX(i));
            emit targetChanged(target);
            break;
        }
    }
}
