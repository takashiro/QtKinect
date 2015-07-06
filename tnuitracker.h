#ifndef TNUITRACKER_H
#define TNUITRACKER_H

#include <QObject>
#include <QPointF>
#include <QVector3D>

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

    QPointF screenPos() const { return m_screenPos; }

    QVector3D realPos() const { return m_realPos; }
    float x() const { return m_realPos.x(); }
    float y() const { return m_realPos.y(); }
    float z() const { return m_realPos.z(); }

signals:
    void screenPosChanged(const QPointF &position);
    void realPosChanged(const QVector3D &position);

protected:
    void handleNewFrame();

    QPointF m_screenPos;
    QVector3D m_realPos;

    static TNuiSkeletonStream *m_skeletonStream;
    NUI_SKELETON_POSITION_INDEX m_target;
};

#endif // TNUITRACKER_H
