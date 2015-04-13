#include "tnuibackgroundremovedcolorstream_p.h"
#include "tnuibackgroundremovedcolorstream.h"
#include "tnuisensor.h"
#include "tnuidepthstream.h"

#include <KinectBackgroundRemoval.h>

TNuiBackgroundRemovedColorStreamPrivate::TNuiBackgroundRemovedColorStreamPrivate(TNuiBackgroundRemovedColorStream *stream)
    : TNuiStream(stream->sensor())
    , stream(stream)
{
    m_frameReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    NuiCreateBackgroundRemovedColorStream(m_sensor->nativeSensor(), &nativeStream);
}

TNuiBackgroundRemovedColorStreamPrivate::~TNuiBackgroundRemovedColorStreamPrivate()
{
    stop();

    nativeStream->Release();
}

bool TNuiBackgroundRemovedColorStreamPrivate::open()
{
    NUI_IMAGE_RESOLUTION colorResolution = (NUI_IMAGE_RESOLUTION) stream->resolution();
    NUI_IMAGE_RESOLUTION depthResolution = (NUI_IMAGE_RESOLUTION) stream->m_depthStream->resolution();
    m_isOpen = (S_OK == nativeStream->Enable(colorResolution, depthResolution, m_frameReadyEvent));
    if (m_isOpen)
        start();
    return m_isOpen;
}

bool TNuiBackgroundRemovedColorStreamPrivate::processNewFrame()
{
    HRESULT hr;
    NUI_BACKGROUND_REMOVED_COLOR_FRAME frame;

    hr = nativeStream->GetNextFrame(0, &frame);
    if (FAILED(hr))
        return false;

    stream->m_dataMutex.lock();
    memcpy_s(stream->m_outputData, stream->m_dataSize, frame.pBackgroundRemovedColorData, frame.backgroundRemovedColorDataLength);
    stream->m_dataMutex.unlock();

    hr = nativeStream->ReleaseFrame(&frame);
    if (FAILED(hr))
        return false;

    return true;
}
