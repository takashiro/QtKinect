#include "tnuidepthstream.h"
#include "tnuisensor.h"

INuiFrameTexture *TNuiDepthStream::readFrameTexture()
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
