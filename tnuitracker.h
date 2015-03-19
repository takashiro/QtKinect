#ifndef TNUITRACKER_H
#define TNUITRACKER_H

#include <QObject>

class TNuiSkeletonStream;
class TNuiImageStream;

class TNuiTracker : public QObject
{
    Q_OBJECT

public:
    TNuiTracker(TNuiSkeletonStream *parent, TNuiImageStream *imageStream);

signals:
    void rightHandMoved(const QPointF &position);

protected:
    void handleNewFrame();

    TNuiSkeletonStream *m_skeletonStream;
    TNuiImageStream *m_imageStream;
};

#endif // TNUITRACKER_H
