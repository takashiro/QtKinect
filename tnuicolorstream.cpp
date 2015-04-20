#include "tnuicolorstream.h"
#include "tnuicolorstreameffect.h"

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
    m_imageSize = dataSize();
    m_image = new uchar[m_imageSize];
}

TNuiColorStream::~TNuiColorStream()
{
    delete[] m_image;
}

void TNuiColorStream::addEffect(TNuiColorStreamEffect *effect)
{
    m_effects.insert(effect);
}

void TNuiColorStream::removeEffect(TNuiColorStreamEffect *effect)
{
    m_effectMutex.lock();
    m_effects.remove(effect);
    m_effectMutex.unlock();
}

bool TNuiColorStream::hasEffect(TNuiColorStreamEffect *effect)
{
    return m_effects.contains(effect);
}

QImage TNuiColorStream::readImage()
{
    d->m_dataMutex.lockForRead();
    if (d->m_outputData == nullptr) {
        d->m_dataMutex.unlock();
        return QImage(640, 480, QImage::Format_RGBA8888);
    }
    memcpy_s(m_image, m_imageSize, data(), dataSize());
    d->m_dataMutex.unlock();

    m_effectMutex.lock();
    foreach (TNuiColorStreamEffect *effect, m_effects)
        effect->draw(m_image, m_imageSize);
    m_effectMutex.unlock();

    uchar *maxi = m_image + 640 * 480 * 4;
    uchar tmp;
    for (uchar *r = m_image, *b = m_image + 2; b < maxi; r += 4, b += 4) {
        tmp = *r;
        *r = *b;
        *b = tmp;
    }

    return QImage(m_image, 640, 480, QImage::Format_RGBA8888);
}
