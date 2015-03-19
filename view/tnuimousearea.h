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
    void onLeftHandMoved(const QPointF &pos);
    void onRightHandMoved(const QPointF &pos);
    void onHandMoved(bool &isUnderHand, const QPointF &pos);

signals:
    void entered();
    void exited();
    void touched();

private:
    bool m_isUnderLeftHand;
    bool m_isUnderRightHand;
    QDateTime m_focusTimeOffset;
};

#endif // TNUIMOUSEAREA_H
