#ifndef TIMAGE_H
#define TIMAGE_H

#include <QImage>
#include <QQuickPaintedItem>

class QTimer;

class TImage : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit TImage(QQuickItem *parent = 0);

    void setImage(const QImage &image);
    virtual void paint(QPainter *painter);

protected:
    QImage m_image;
};

#endif // TIMAGE_H
