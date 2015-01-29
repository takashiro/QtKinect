#include "tvideo.h"

#include <QPainter>
#include <QTimer>

TVideo::TVideo(QQuickItem *parent)
    : QQuickPaintedItem(parent), m_frame("ui/image/default-background.jpg")
{
}

void TVideo::setFrame(const QImage &image)
{
    m_frame = image;
    update();
}

void TVideo::paint(QPainter *painter)
{
    QRect rect(0, 0, width(), height());
    painter->drawImage(rect, m_frame);
}

class TVideoAdder
{
public:
    TVideoAdder()
    {
        qmlRegisterType<TVideo>("Kinect", 1, 0, "TVideo");
    }
};
TVideoAdder adder;
