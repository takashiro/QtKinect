#include "tnuisensor.h"
#include "windowsutil.h"
#include "tnuiimagestream.h"
#include "tnuiskeletonstream.h"

TNuiSensor::TNuiSensor(INuiSensor *sensor, QObject *parent)
    : QObject(parent)
    , m_state(InitializingState)
    , m_sensor(sensor)
    , m_skeletonStream(nullptr)
{
    memset(m_imageStream, 0, sizeof(m_imageStream));

    m_sensor->AddRef();
    m_deviceConnectionId = WindowsUtil::toString(m_sensor->NuiDeviceConnectionId());
    connect(this, &TNuiSensor::stateChanged, this, &TNuiSensor::_onStateChanged);
}

TNuiSensor::~TNuiSensor()
{
    m_sensor->Release();
}

TNuiImageStream *TNuiSensor::createImageStream(TNuiImageStream::ImageType type)
{
    TNuiImageStream *&stream = m_imageStream[type];
    if (stream == nullptr) {
        stream = new TNuiImageStream(this, type);
        stream->open();
    }
    return stream;
}

TNuiSkeletonStream *TNuiSensor::createSkeletonStream(TNuiSkeletonStream::TrackingFlags flags)
{
    if (m_skeletonStream == nullptr) {
        m_skeletonStream = new TNuiSkeletonStream(this, flags);
        m_skeletonStream->open();
    } else if (m_skeletonStream->flags() != flags) {
        m_skeletonStream->setFlags(flags);
        m_skeletonStream->reopen();
    }

    return m_skeletonStream;
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

bool TNuiSensor::_openImageStream(TNuiImageStream *stream, ulong imageFrameFlags, ulong frameLimit)
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

bool TNuiSensor::_readImageFrame(TNuiImageStream *stream, ulong msecondsToWait, NUI_IMAGE_FRAME &ppcImageFrame)
{
    return S_OK == m_sensor->NuiImageStreamGetNextFrame(stream->m_streamHandle, msecondsToWait, &ppcImageFrame);
}

bool TNuiSensor::_releaseImageFrame(TNuiImageStream *stream, NUI_IMAGE_FRAME &frame)
{
    return S_OK == m_sensor->NuiImageStreamReleaseFrame(stream->m_streamHandle, &frame);
}

bool TNuiSensor::_openSkeletionStream(TNuiSkeletonStream *stream, ulong flags)
{
    return S_OK == m_sensor->NuiSkeletonTrackingEnable(stream->m_frameReadyEvent, flags);
}

bool TNuiSensor::_readSkeletonFrame(ulong msecondsToWait, NUI_SKELETON_FRAME &frame)
{
    return S_OK == m_sensor->NuiSkeletonGetNextFrame(msecondsToWait, &frame);
}

bool TNuiSensor::_closeSkeletonStream()
{
    return S_OK == m_sensor->NuiSkeletonTrackingDisable();
}
