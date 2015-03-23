#ifndef TNUIMOUSE_H
#define TNUIMOUSE_H

#include <QQuickItem>

class TNuiTracker;

class TNuiTrackerItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString target READ target WRITE setTarget NOTIFY targetChanged)

public:
    TNuiTrackerItem(QQuickItem *parent = 0);
    ~TNuiTrackerItem();

    QString target() const;
    void setTarget(const QString &target);

signals:
    void targetChanged(const QString &newTarget);

private:
    TNuiTracker *m_tracker;
};

#endif // TNUIMOUSE_H
