#include "tnuisensor.h"
#include "windowsutil.h"

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

void TNuiSensor::_onStateChanged(State state)
{
    if (state == NotConnectedState)
        emit disconnected();
    else if (state == ConnectedState)
        emit connected();
}

void TNuiSensor::_updateState()
{
    State state = (State) m_sensor->NuiStatus();
    if (state != m_state) {
        m_state = state;
        emit stateChanged(state);
    }
}
