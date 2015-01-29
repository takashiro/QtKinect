#ifndef TVIDEO_H
#define TVIDEO_H

#include <QImage>
#include <QQuickPaintedItem>

class QTimer;

class TVideo : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit TVideo(QQuickItem *parent = 0);

    void setFrame(const QImage &image);
    virtual void paint(QPainter *painter);

protected:
    void testFrame();

    QImage m_frame;
    QTimer *m_timer;
};

#endif // TVIDEO_H
