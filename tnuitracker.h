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
    TNuiTracker(TNuiSensor *sensor, NUI_SKELETON_POSITION_INDEX pos = NUI_SKELETON_POSITION_HAND_RIGHT);

signals:
    void moved(const QPointF &position);

protected:
    void handleNewFrame();

    TNuiSkeletonStream *m_skeletonStream;
    TNuiImageStream *m_colorStream;
    NUI_SKELETON_POSITION_INDEX m_pos;
};

#endif // TNUITRACKER_H
