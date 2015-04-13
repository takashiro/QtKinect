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
}

TNuiSensor::~TNuiSensor()
{
    m_sensor->Release();
}

void TNuiSensor::initialize(uint flags)
{
     HRESULT hr = m_sensor->NuiInitialize(flags);
     if (hr == S_OK)
         _updateState();
}

void TNuiSensor::_updateState()
{
    m_state = (State) m_sensor->NuiStatus();
    emit stateChanged();

    if (m_state == NotConnectedState)
        emit disconnected();
    else if (m_state == ConnectedState)
        emit connected();
}
