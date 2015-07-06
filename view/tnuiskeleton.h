#ifndef TNUISKELETON_H
#define TNUISKELETON_H

#include <QQuickItem>
#include <QVector3D>

class TNuiTracker;

class TNuiSkeleton : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QVector3D realPos READ realPos NOTIFY realPosChanged)

public:
    TNuiSkeleton(QQuickItem *parent = 0);
    ~TNuiSkeleton();

    QString target() const;
    void setTarget(const QString &target);

    QVector3D realPos() const { return m_realPos; }

signals:
    void targetChanged();
    void realPosChanged();

private:
    void setRealPos(const QVector3D &pos);

    TNuiTracker *m_tracker;
    QVector3D m_realPos;
};

#endif // TNUISKELETON_H
