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

class TNuiSensorStarter: public QThread
{
    friend class TNuiSensor;

    TNuiSensorStarter(TNuiSensor *sensor, uint flags)
        : QThread(sensor)
        , sensor(sensor)
        , flags(flags)
    {
    }

    void run()
    {
        HRESULT hr = sensor->nativeSensor()->NuiInitialize(flags);
        if (hr == S_OK)
            sensor->updateState();
    }

    TNuiSensor *sensor;
    uint flags;
};

void TNuiSensor::initialize(uint flags)
{
    TNuiSensorStarter *starter = new TNuiSensorStarter(this, flags);
    connect(starter, &TNuiSensorStarter::finished, starter, &TNuiSensorStarter::deleteLater);
    starter->start();
}

void TNuiSensor::updateState()
{
    m_state = (State) m_sensor->NuiStatus();
    emit stateChanged();

    if (m_state == NotConnectedState)
        emit disconnected();
    else if (m_state == ConnectedState)
        emit connected();
}

QString TNuiSensor::stateString() const
{
    switch (m_state) {
    case TNuiSensor::ConnectedState:
        return "connected";
    case TNuiSensor::InitializingState:
        return "initializing";
    case TNuiSensor::NotConnectedState:
        return "not_connected";
    case TNuiSensor::NotGenuine:
        return "invalid";
    case TNuiSensor::NotSupportedState:
        return "not_supported";
    case TNuiSensor::InsufficientBandwidthState:
        return "insufficient_bandwidth";
    case TNuiSensor::NotPoweredState:
        return "unpowered";
    case TNuiSensor::NotReadyState:
        return "not_ready";
    default:
        return "unkown";
    }
}
