#include "tnuisensormanager.h"
#include "tnuisensor.h"
#include "windowsutil.h"

#include <QCoreApplication>

TNuiSensorManager *SensorManager = TNuiSensorManager::instance();

void CALLBACK TNuiSensorManager::ChangeStatusCallback(HRESULT hrStatus, const OLECHAR *winInstanceName, const OLECHAR *, void *userData)
{
    TNuiSensorManager *manager = reinterpret_cast<TNuiSensorManager *>(userData);
    QString instanceName = WindowsUtil::toString(winInstanceName);
    TNuiSensor *sensor = manager->m_sensors.value(instanceName, nullptr);

    if (SUCCEEDED(hrStatus)) {
        if (sensor == nullptr) {
            INuiSensor *pNuiSensor = nullptr;
            if (SUCCEEDED(NuiCreateSensorById(winInstanceName, &pNuiSensor))) {
                sensor = new TNuiSensor(pNuiSensor);
                manager->m_sensors.insert(instanceName, sensor);
                manager->newSensorConnected(sensor);
            }
            pNuiSensor->Release();
        } else {
            sensor->_updateState();
        }
    } else {
        if (sensor != nullptr) {
            manager->m_sensors.remove(instanceName);
            delete sensor;
        }
    }
}

TNuiSensorManager::TNuiSensorManager(QObject *parent)
    : QObject(parent)
{
    NuiSetDeviceStatusCallback(ChangeStatusCallback, this);
}

TNuiSensorManager *TNuiSensorManager::instance()
{
    static TNuiSensorManager manager;
    return &manager;
}
