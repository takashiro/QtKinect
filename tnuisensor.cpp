#include "tnuisensor.h"
#include "windowsutil.h"

#include <QMap>

TNuiSensor::TNuiSensor(INuiSensor *sensor, QObject *parent)
    : QObject(parent)
    , m_state(InitializingState)
    , m_sensor(sensor)
{
    m_sensor->AddRef();

    connect(this, &TNuiSensor::stateChanged, this, &TNuiSensor::_onStateChanged);
}

TNuiSensor::~TNuiSensor()
{
    m_sensor->Release();
}

TNuiSensor::_onStateChanged(State state)
{
    if (state == NotConnectedState)
        emit disconnected();
    else
        emit connected();
}

TNuiSensor::_setState(State state)
{
    if (state != m_state) {
        m_state = state;
        emit stateChanged(state);
    }
}
