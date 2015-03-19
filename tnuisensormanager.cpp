#include "tnuisensormanager.h"
#include "tnuisensor.h"
#include "windowsutil.h"
#include "stdafx.h"

#include <QCoreApplication>

TNuiSensorManager *SensorManager = nullptr;

void CALLBACK TNuiSensorManager::ChangeStatusCallback(HRESULT hrStatus, const OLECHAR *instanceName, const OLECHAR *, void *userData)
{
    TNuiSensorManager *manager = reinterpret_cast<TNuiSensorManager *>(userData);
    if (SUCCEEDED(hrStatus)) {
        foreach (TNuiSensor *sensor, manager->m_sensors) {
            if (sensor->deviceConnectionId() != WindowsUtil::toString(instanceName))
                continue;

            if (sensor == nullptr) {
                //Insert a new TNuiSensor
                INuiSensor *pNuiSensor = nullptr;
                if (SUCCEEDED(NuiCreateSensorById(instanceName, &pNuiSensor))) {
                    sensor = manager->_addSensor(pNuiSensor);
                }
                pNuiSensor->Release();
            } else {
                //Simply update the state
                sensor->_updateState();
            }
            break;
        }
    } else {
        foreach (TNuiSensor *sensor, manager->m_sensors) {
            if (sensor->deviceConnectionId() != WindowsUtil::toString(instanceName))
                continue;
            sensor->_updateState();
            break;
        }
    }
}

TNuiSensorManager::TNuiSensorManager(QObject *parent)
    : QObject(parent)
{
    int iCount = 0;
    HRESULT hr = NuiGetSensorCount(&iCount);
    if (FAILED(hr)) {
        qFatal("NuiGetSensorCount Failed.");
        return;
    }

    for (int i = 0; i < iCount; ++i) {
        INuiSensor* pNuiSensor = nullptr;

        if (SUCCEEDED(NuiCreateSensorByIndex(i, &pNuiSensor))) {
            _addSensor(pNuiSensor);
        }

        SafeRelease(pNuiSensor);
    }

    NuiSetDeviceStatusCallback(ChangeStatusCallback, this);
}

TNuiSensorManager *TNuiSensorManager::instance()
{
    static TNuiSensorManager manager;
    return &manager;
}

TNuiSensorManager::~TNuiSensorManager()
{
    foreach (TNuiSensor *sensor, m_sensors) {
        //@todo: it takes an extremely long time to shutdown()
        //sensor->shutdown();
        delete sensor;
    }
}

TNuiSensor *TNuiSensorManager::_addSensor(INuiSensor *pNuiSensor)
{
    TNuiSensor *sensor = new TNuiSensor(pNuiSensor);
    m_sensors << sensor;
    emit newSensorConnected(sensor);
    return sensor;
}
