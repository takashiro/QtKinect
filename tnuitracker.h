#ifndef TNUITRACKER_H
#define TNUITRACKER_H

#include <QObject>

class TNuiSkeletonStream;
class TNuiImageStream;
class TNuiSensor;

class TNuiTracker : public QObject
{
    Q_OBJECT

public:
    TNuiTracker(TNuiSensor *sensor, TNuiImageStream *imageStream);

signals:
    void moved(const QPointF &position);

protected:
    void handleNewFrame();

    TNuiSkeletonStream *m_skeletonStream;
    TNuiImageStream *m_imageStream;
};

#endif // TNUITRACKER_H
