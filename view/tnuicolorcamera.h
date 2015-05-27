#ifndef TNUICOLORCAMERA_H
#define TNUICOLORCAMERA_H

#include <QQuickItem>

class TNuiColorStream;
class QSGTexture;
class TNuiBackgroundRemovalEffect;

class TNuiColorCamera : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(bool backgroundRemoved READ backgroundRemoved WRITE setBackgroundRemoved NOTIFY backgroundRemovedChanged)

public:
    TNuiColorCamera(QQuickItem *parent = 0);
    ~TNuiColorCamera();

    bool backgroundRemoved() const;
    void setBackgroundRemoved(bool removed);

signals:
    void backgroundRemovedChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);

    TNuiColorStream *m_stream;
    QMutex m_textureMutex;
    TNuiBackgroundRemovalEffect *m_backgroundRemovalEffect;
};

#endif // TNUICOLORCAMERA_H
