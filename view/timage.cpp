#include "timage.h"

#include <QPainter>
#include <QTimer>

TImage::TImage(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
}

void TImage::setImage(const QImage &image)
{
    m_image = image;
    update();
}

void TImage::paint(QPainter *painter)
{
    QRect rect(0, 0, width(), height());
    painter->drawImage(rect, m_image);
}
