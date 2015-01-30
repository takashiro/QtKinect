#include "tnuiimage.h"

#include <QPainter>

TNuiImage::TNuiImage(QQuickItem *parent)
    : TImage(parent)
{
}

void TNuiImage::setImage(const TNuiImageBuffer *frame)
{
    const BYTE *data = frame->buffer();
    const uint width = frame->width();
    const uint height = frame->height();

    m_image = QImage(width, height, QImage::Format_RGBA8888);
    for (uint i = 0; i < width; i++) {
        for (uint j = 0; j < height; j++) {
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
