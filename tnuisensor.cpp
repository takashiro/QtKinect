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
