#ifndef TNUICOLORCAMERA_H
#define TNUICOLORCAMERA_H

#include <QQuickItem>
#include <QImage>

class TNuiColorStream;
class QSGTexture;

class TNuiColorCamera : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(bool backgroundRemoved READ backgroundRemoved WRITE setBackgroundRemoved NOTIFY backgroundRemovedChanged)

public:
    TNuiColorCamera(QQuickItem *parent = 0);

    bool backgroundRemoved() const;
    void setBackgroundRemoved(bool removed);

signals:
    void backgroundRemovedChanged();

protected:
    void tryOpenStream();
    void updateFrame();
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);

    TNuiColorStream *m_stream;
    QImage m_image;
    QSGTexture *m_texture;
};

#endif // TNUICOLORCAMERA_H
