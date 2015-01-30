#ifndef TNUISENSORMANAGER_H
#define TNUISENSORMANAGER_H

#include <QString>
#include <QObject>

#include <Windows.h>

class TNuiSensor;
struct INuiSensor;

class TNuiSensorManager : public QObject
{
    Q_OBJECT

public:
    static void CALLBACK ChangeStatusCallback(HRESULT, const OLECHAR *winInstanceName, const OLECHAR *, void *userData);
    static TNuiSensorManager *instance();

    ~TNuiSensorManager();
    TNuiSensor *sensor() const {return m_sensors.first();}

signals:
    void newSensorConnected(TNuiSensor *sensor);

private:
    TNuiSensorManager(QObject *parent = 0);

protected:
    TNuiSensor *_addSensor(INuiSensor *sensor);

    QList<TNuiSensor *> m_sensors;
};

extern TNuiSensorManager *SensorManager;

#endif // TNUISENSORMANAGER_H
