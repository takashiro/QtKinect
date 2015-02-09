#include "tnuisensor.h"
#include "windowsutil.h"
#include "tnuiimagestream.h"
#include "tnuiskeletonstream.h"

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

bool TNuiSensor::openImageStream(TNuiImageStream *stream, ulong imageFrameFlags, ulong frameLimit)
{
    HRESULT hr = m_sensor->NuiImageStreamOpen(
                (NUI_IMAGE_TYPE) stream->m_imageType,
                (NUI_IMAGE_RESOLUTION) stream->m_imageResolution,
                imageFrameFlags,
                frameLimit,
                stream->m_frameReadyEvent,
                &stream->m_streamHandle);
    return SUCCEEDED(hr);
}

bool TNuiSensor::readImageFrame(TNuiImageStream *stream, ulong msecondsToWait, NUI_IMAGE_FRAME &ppcImageFrame)
{
    return S_OK == m_sensor->NuiImageStreamGetNextFrame(stream->m_streamHandle, msecondsToWait, &ppcImageFrame);
}

bool TNuiSensor::releaseImageFrame(TNuiImageStream *stream, NUI_IMAGE_FRAME &frame)
{
    return S_OK == m_sensor->NuiImageStreamReleaseFrame(stream->m_streamHandle, &frame);
}

bool TNuiSensor::openSkeletionStream(TNuiSkeletonStream *stream, ulong flags)
{
    return S_OK == m_sensor->NuiSkeletonTrackingEnable(stream->m_frameReadyEvent, flags);
}

bool TNuiSensor::readSkeletonFrame(ulong msecondsToWait, NUI_SKELETON_FRAME &frame)
{
    return S_OK == m_sensor->NuiSkeletonGetNextFrame(msecondsToWait, &frame);
}

bool TNuiSensor::closeSkeletonStream()
{
    return S_OK == m_sensor->NuiSkeletonTrackingDisable();
}
