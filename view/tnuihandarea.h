#ifndef TNUIHANDAREA_H
#define TNUIHANDAREA_H

#include <QQuickItem>

class QTimer;

class TNuiHandArea : public QQuickItem
{
    Q_OBJECT

public:
    TNuiHandArea(QQuickItem *parent = 0);
    ~TNuiHandArea();

protected slots:
    void onLeftHandMoved(const QPointF &pos);
    void onRightHandMoved(const QPointF &pos);
    void onHandMoved(bool &isUnderHand, const QPointF &pos);

    void checkLongTouch();

signals:
    void entered();//Either hand enters
    void exited(); //Either hand leaves

    void longTouched();//Either hand touches for over 1.5 seconds

private:
    bool m_isUnderLeftHand;
    bool m_isUnderRightHand;

    QTimer *m_longTouchTimer;
};

#endif // TNUIHANDAREA_H
