#ifndef TNUIMOUSEAREA_H
#define TNUIMOUSEAREA_H

#include <QQuickItem>
#include <QDateTime>

class TNuiSkeletonStream;
class TNuiColorStream;
class TNuiTracker;

class TNuiMouseArea : public QQuickItem
{
    Q_OBJECT

public:
    TNuiMouseArea(QQuickItem *parent = 0);
    ~TNuiMouseArea();

protected slots:
    void onMouseMoved(const QPointF &pos);

signals:
    void entered();
    void exited();
    void touched();

private:
    TNuiSkeletonStream *m_skeletonStream;
    TNuiColorStream *m_colorStream;
    TNuiTracker *m_tracker;

    bool m_isFocused;
    QDateTime m_focusTimeOffset;
};

#endif // TNUIMOUSEAREA_H
