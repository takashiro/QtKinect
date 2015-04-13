#include "tnuisensormanager.h"
#include "tnuisensor.h"
#include "windowsutil.h"

#include <QCoreApplication>

TNuiSensorManager *SensorManager = nullptr;

void CALLBACK TNuiSensorManager::ChangeStatusCallback(HRESULT hrStatus, const OLECHAR *oleInstanceName, const OLECHAR *, void *userData)
{
    QString instanceName = WindowsUtil::toString(oleInstanceName);
    TNuiSensorManager *manager = reinterpret_cast<TNuiSensorManager *>(userData);
    if (SUCCEEDED(hrStatus)) {
        bool isNewSensor = true;
        foreach (TNuiSensor *sensor, manager->m_sensors) {
            if (sensor->deviceConnectionId() == instanceName) {
                //Simply update the state
                sensor->_updateState();
                isNewSensor = false;
                break;
            }
        }

        if (isNewSensor) {
            //Insert a new TNuiSensor
            INuiSensor *pNuiSensor = nullptr;
            if (SUCCEEDED(NuiCreateSensorById(oleInstanceName, &pNuiSensor))) {
                manager->_addSensor(pNuiSensor);
                pNuiSensor->Release();
            }
        }

    } else {
        foreach (TNuiSensor *sensor, manager->m_sensors) {
            if (sensor->deviceConnectionId() != instanceName)
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
            if (pNuiSensor != nullptr) {
                 _addSensor(pNuiSensor);
                 pNuiSensor->Release();
            }
        }
    }

    NuiSetDeviceStatusCallback(ChangeStatusCallback, this);
}

TNuiSensorManager *TNuiSensorManager::instance()
{
    static TNuiSensorManager *manager = new TNuiSensorManager(qApp);
    return manager;
}

TNuiSensorManager::~TNuiSensorManager()
{
    //@todo: it takes an extremely long time to shutdown()
    /*foreach (TNuiSensor *sensor, m_sensors)
        sensor->shutdown();*/
}

TNuiSensor *TNuiSensorManager::_addSensor(INuiSensor *pNuiSensor)
{
    TNuiSensor *sensor = new TNuiSensor(pNuiSensor, this);
    m_sensors << sensor;
    emit newSensorConnected(sensor);
    return sensor;
}
