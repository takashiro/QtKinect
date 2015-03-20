#ifndef TNUIMOUSE_H
#define TNUIMOUSE_H

#include <QQuickItem>

class TNuiTracker;

class TNuiTrackerItem : public QQuickItem
{
    Q_OBJECT

public:
    TNuiTrackerItem(QQuickItem *parent = 0);
    ~TNuiTrackerItem();

private:
    TNuiTracker *m_tracker;
};

#endif // TNUIMOUSE_H
