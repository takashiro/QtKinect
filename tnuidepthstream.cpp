#include "tnuidepthstream.h"
#include "tnuisensor.h"

INuiFrameTexture *TNuiDepthStream::readFrameTexture()
{
    INuiFrameTexture *texture;
    BOOL nearMode = hasFlag(EnableNearMode) ? TRUE : FALSE;

    // Attempt to get the extended depth texture
    HRESULT hr = m_sensor->nativeSensor()->NuiImageFrameGetDepthImagePixelFrameTexture(m_streamHandle, &m_frame, &nearMode, &texture);
    if (FAILED(hr))
        return nullptr;

    return texture;
}
