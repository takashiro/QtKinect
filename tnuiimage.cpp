#include "tnuiimage.h"

#include <QPainter>

TNuiImage::TNuiImage(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
}

void TNuiImage::setImage(const TNuiImageBuffer *frame)
{
    const BYTE *data = frame->buffer();
    const width = frame->width();
    const height = frame->height();

    m_image = QImage(width, height, QImage::Format_RGBA8888);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            uchar b = *data;
            data++;
            uchar g = *data;
            data++;
            uchar r = *data;
            data++;
            uchar a = *data;
            data++;
            uint color = r;
            color <<= 8;
            color |= g;
            color <<= 8;
            color |= b;
            color <<= 8;
            color |= a;
            m_image.setPixel(i, j, color);
        }
    }

    update();
}

class TNuiImageAdder
{
public:
    TNuiImageAdder(){
        qmlRegisterType<TNuiImage>("Kinect", 1, 0, "TNuiImage");
    }
};
TNuiImageAdder adder;
