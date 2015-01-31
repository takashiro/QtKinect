#include "tnuisensor.h"
#include "windowsutil.h"
#include "tnuistream.h"

#include <QMap>

TNuiSensor::TNuiSensor(INuiSensor *sensor, QObject *parent)
    : QObject(parent)
    , m_state(InitializingState)
    , m_sensor(sensor)
{
    m_sensor->AddRef();
    m_deviceConnectionId = WindowsUtil::toString(m_sensor->NuiDeviceConnectionId());
    connect(this, &TNuiSensor::stateChanged, this, &TNuiSensor::_onStateChanged);
}

TNuiSensor::~TNuiSensor()
{
    m_sensor->Release();
}

void TNuiSensor::_onStateChanged()
{
    if (m_state == NotConnectedState)
        emit disconnected();
    else if (m_state == ConnectedState)
        emit connected();
}

void TNuiSensor::_updateState()
{
    m_state = (State) m_sensor->NuiStatus();
    emit stateChanged();
}

bool TNuiSensor::openImageStream(TNuiStream *stream, ulong imageFrameFlags, ulong frameLimit)
{
    HRESULT hr = m_sensor->NuiImageStreamOpen(
                stream->m_imageType,
                stream->m_imageResolution,
                imageFrameFlags,
                frameLimit,
                stream->m_frameReadyEvent,
                &stream->m_streamHandle);
    return SUCCEEDED(hr);
}

bool TNuiSensor::readNextFrame(TNuiStream *stream, ulong msecondsToWait, NUI_IMAGE_FRAME &ppcImageFrame)
{
    return S_OK == m_sensor->NuiImageStreamGetNextFrame(stream->m_streamHandle, msecondsToWait, &ppcImageFrame);
}

bool TNuiSensor::releaseFrame(TNuiStream *stream, NUI_IMAGE_FRAME &frame)
{
    return S_OK == m_sensor->NuiImageStreamReleaseFrame(stream->m_streamHandle, &frame);
}
