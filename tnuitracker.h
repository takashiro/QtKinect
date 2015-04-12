#ifndef TNUITRACKER_H
#define TNUITRACKER_H

#include <QObject>

#include <Windows.h>
#include <NuiApi.h>

class TNuiSkeletonStream;
class TNuiImageStream;
class TNuiSensor;

class TNuiTracker : public QObject
{
    Q_OBJECT

public:
    TNuiTracker(TNuiSensor *sensor, NUI_SKELETON_POSITION_INDEX target = NUI_SKELETON_POSITION_COUNT);

    void setTarget(NUI_SKELETON_POSITION_INDEX target) { m_target = target; }
    NUI_SKELETON_POSITION_INDEX target() const { return m_target; }

signals:
    void moved(const QPointF &position);

protected:
    void handleNewFrame();

    TNuiSkeletonStream *m_skeletonStream;
    NUI_SKELETON_POSITION_INDEX m_target;
};

#endif // TNUITRACKER_H
