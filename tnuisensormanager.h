#ifndef TNUISENSORMANAGER_H
#define TNUISENSORMANAGER_H

#include <QString>
#include <QMap>
#include <QObject>

#include <Windows.h>

class TNuiSensor;

class TNuiSensorManager : public QObject
{
    Q_OBJECT

public:
    static void CALLBACK ChangeStatusCallback(HRESULT, const OLECHAR *winInstanceName, const OLECHAR *, void *userData);
    static TNuiSensorManager *instance();

    TNuiSensor *sensor() const {return m_sensors.first();}

signals:
    void newSensorConnected(TNuiSensor *sensor);

private:
    TNuiSensorManager(QObject *parent = 0);

protected:
    QMap<QString, TNuiSensor *> m_sensors;
};

extern TNuiSensorManager *SensorManager;

#endif // TNUISENSORMANAGER_H
