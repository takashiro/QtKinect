#include "tnuisensor.h"

TNuiSensor::TNuiSensor(INuiSensor *sensor, QObject *parent)
    : QObject(parent)
    , m_state(InitializingState)
    , m_sensor(sensor)
{
    m_sensor->AddRef();
}

TNuiSensor::~TNuiSensor()
{
    m_sensor->Release();
}
