#include "tnuidepthstream.h"
#include "tnuisensor.h"

TNuiDepthStreamInternal::TNuiDepthStreamInternal(TNuiSensor *sensor, QObject *parent)
    : TNuiImageStreamInternal(sensor, parent)
{
}

INuiFrameTexture *TNuiDepthStreamInternal::readFrameTexture()
{
    INuiFrameTexture *texture;
    BOOL inNearMode;

    // Attempt to get the extended depth texture
    HRESULT hr = m_sensor->nativeSensor()->NuiImageFrameGetDepthImagePixelFrameTexture(m_streamHandle, &m_frame, &inNearMode, &texture);
    if (FAILED(hr))
        return nullptr;

    m_inNearMode = (inNearMode == TRUE);

    return texture;
}

QPointer<TNuiDepthStreamInternal> TNuiDepthStream::d = nullptr;

TNuiDepthStream::TNuiDepthStream(TNuiSensor *sensor, bool enablePlayerIndex)
    : TNuiImageStream(sensor)
{
    if (d == nullptr)
        d = new TNuiDepthStreamInternal(sensor);
    TNuiImageStream::setInternal(d);
    d->m_type = enablePlayerIndex ? DepthAndPlayerIndex : Depth;
}
