#ifndef TNUIMOUSE_H
#define TNUIMOUSE_H

#include <QQuickItem>

class TNuiSkeletonStream;
class TNuiColorStream;
class TNuiTracker;

class TNuiMouse : public QQuickItem
{
public:
    TNuiMouse(QQuickItem *parent = 0);
    ~TNuiMouse();

protected:
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);

private:
    TNuiSkeletonStream *m_skeletonStream;
    TNuiColorStream *m_colorStream;
    TNuiTracker *m_tracker;
};

#endif // TNUIMOUSE_H
