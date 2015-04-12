#include "tnuicolorstream.h"

QImage TNuiColorStream::readImage()
{
    QImage image(640, 480, QImage::Format_RGBA8888);
    int k = 0;
    uint color;
    uchar *rgba = reinterpret_cast<uchar *>(&color);

    m_dataMutex.lock();
    if (m_outputData == nullptr)
        return image;
    for (int j = 0; j < 480; j++) {
        for (int i = 0; i < 640; i++) {
            rgba[0] = m_outputData[k++];
            rgba[1] = m_outputData[k++];
            rgba[2] = m_outputData[k++];
            rgba[3] = m_outputData[k++];
            image.setPixel(i, j, color);
        }
    }
    m_dataMutex.unlock();
    return image;
}

INuiFrameTexture *TNuiColorStream::readFrameTexture()
{
    return m_frame.pFrameTexture;
}
