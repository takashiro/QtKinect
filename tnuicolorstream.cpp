#include "tnuicolorstream.h"

INuiFrameTexture *TNuiColorStreamInternal::readFrameTexture()
{
    return m_frame.pFrameTexture;
}

QPointer<TNuiColorStreamInternal> TNuiColorStream::d = nullptr;

TNuiColorStream::TNuiColorStream(TNuiSensor *sensor)
    : TNuiImageStream(sensor)
{
    if (d == nullptr)
        d = new TNuiColorStreamInternal(sensor);
    TNuiImageStream::setInternal(d);
    d->m_type = Color;
}

QImage TNuiColorStream::readImage()
{
    QImage image(640, 480, QImage::Format_RGBA8888);
    int k = 0;
    uint color;
    uchar *rgba = reinterpret_cast<uchar *>(&color);

    d->m_dataMutex.lockForRead();
    if (d->m_outputData == nullptr)
        return image;
    for (int j = 0; j < 480; j++) {
        for (int i = 0; i < 640; i++) {
            rgba[0] = d->m_outputData[k++];
            rgba[1] = d->m_outputData[k++];
            rgba[2] = d->m_outputData[k++];
            rgba[3] = d->m_outputData[k++];
            image.setPixel(i, j, color);
        }
    }
    d->m_dataMutex.unlock();
    return image;
}
