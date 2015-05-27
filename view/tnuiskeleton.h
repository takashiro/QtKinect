#ifndef TNUISKELETON_H
#define TNUISKELETON_H

#include <QQuickItem>

class TNuiTracker;

class TNuiSkeleton : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString target READ target WRITE setTarget NOTIFY targetChanged)

public:
    TNuiSkeleton(QQuickItem *parent = 0);
    ~TNuiSkeleton();

    QString target() const;
    void setTarget(const QString &target);

signals:
    void targetChanged(const QString &newTarget);

private:
    TNuiTracker *m_tracker;
};

#endif // TNUISKELETON_H
