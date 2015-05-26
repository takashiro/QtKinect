#ifndef TNUIHANDAREA_H
#define TNUIHANDAREA_H

#include <QQuickItem>

class TNuiTracker;
class QTimer;

class TNuiHandArea : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(bool isUnderLeftHand READ isUnderLeftHand)
    Q_PROPERTY(bool isUnderRightHand READ isUnderRightHand)
    Q_PROPERTY(bool isPressed READ isPressed)
    Q_PROPERTY(float pressDownRange READ pressDownRange WRITE setPressDownRange)
    Q_PROPERTY(float pressUpRange READ pressUpRange WRITE setPressUpRange)
    Q_PROPERTY(float handZ READ handZ NOTIFY handZChanged)
    Q_PROPERTY(float initialHandZ READ initialHandZ)

public:
    TNuiHandArea(QQuickItem *parent = 0);
    ~TNuiHandArea();

    bool isUnderLeftHand() const { return m_isUnderLeftHand; }
    bool isUnderRightHand() const { return m_isUnderRightHand; }
    bool isPressed() const { return m_isPressed; }

    float pressDownRange() const { return m_pressDownRange; }
    void setPressDownRange(float range) { m_pressDownRange = range; }

    float pressUpRange() const { return m_pressUpRange; }
    void setPressUpRange(float range) { m_pressUpRange = range;}

    float handZ() const { return m_handZ; }
    float initialHandZ() const { return m_initialHandZ; }

protected:
    void resetState();

    void onLeftHandMoved(const QPointF &pos);
    void onRightHandMoved(const QPointF &pos);
    void onHandMoved(bool &isUnderHand, const QPointF &pos);
    void onTrackerZChanged(float z);

signals:
    void entered();
    void exited();
    void longTouched();//Either hand touches for over 1.5 seconds

    void pressDown();
    void pressUp();
    void longPressed();//Either hand presses for over 1.5 seconds

    void gripped();
    void ungripped();

    void handZChanged();

private:
    bool m_isUnderLeftHand;
    bool m_isUnderRightHand;
    bool m_isPressed;

    QTimer *m_longTouchTimer;
    QTimer *m_longPressTimer;

    float m_initialHandZ;
    float m_handZ;
    float m_pressDownRange;
    float m_pressUpRange;

    static TNuiTracker *m_leftTracker;
    static TNuiTracker *m_rightTracker;
};

#endif // TNUIHANDAREA_H
