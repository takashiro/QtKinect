#ifndef NUISENSORSELECTOR_H
#define NUISENSORSELECTOR_H

#include <QObject>
#include <QString>
#include <QList>

struct INuiSensor;

class TNuiSensorSelector
{
public:
    enum ChangeFlag{
        NoneChangedFlag,
        SensorChangedFlag,
        StatusChangedFlag
    };

    enum Status
    {
        // Chooser has not been started or it has been stopped
        NoneStatus = 0x00000000,

        // Don't have a sensor yet, a sensor is initializing, you may not get it
        InitializingStatus = 0x00000001,

        // This NuiSensorChooser has a connected and started sensor.
        StartedStatus = 0x00000002,

        // There are no sensors available on the system.  If one shows up
        // we will try to use it automatically.
        NoAvailableSensorsStatus = 0x00000004,

        // Available sensor is in use by another application
        ConflictStatus = 0x00000008,

        // The available sensor is not powered.  If it receives power we
        // will try to use it automatically.
        NotPoweredStatus = 0x00000010,

        // There is not enough bandwidth on the USB controller available
        // for this sensor.
        InsufficientBandwidthStatus = 0x00000020,

        // Available sensor is not genuine.
        NotGenuineStatus = 0x00000040,

        // Available sensor is not supported
        NotSupportedStatus = 0x00000080,

        // Available sensor has an error
        ErrorStatus = 0x00000100,
    };

    TNuiSensorSelector();
    ~TNuiSensorSelector();

    ChangeFlag onNuiStatusChanged();

    // Get the current connected Nui sensor.
    // dwFlags: The flags to initialize the returned sensor.
    INuiSensor *getSensor(quint32 flags);

    // Clients should call this method to resolve the conflicts or find a new sensor when current
    // sensor status is SensorConflict.
    ChangeFlag tryResolveConflict();

    // Retrieve the status of the current connected sensor or the reason why we cannot get a sensor.
    quint32 getStatus() const;

    // The NuiSensorChooser will try to find and connect to the sensor specified by this method.
    // strRequiredSensorId: The id string of the sensor which users want to connect. NULL means anyone is Ok.
    ChangeFlag setDesiredSensor(const QString &requiredSensorId);

private:

    /// Called when we don't have a sensor or possibly have the wrong sensor,
    /// and we want to see if we can get one

    /// <returns>The device change type</returns>
    ChangeFlag tryFindNuiSensor();

    /// Find the required sensor and verify if it is operateable

    /// <returns>The device change type</returns>
    ChangeFlag tryFindRequiredNuiSensor();

    /// Enumerate all sensors and try to find an operateable sensor

    /// <returns>The device change type</returns>
    ChangeFlag tryFindAnyOneNuiSensor();

    /// Update the sensor instance and its status, return if sensor and/or status is changed

    /// <param name="vSensors">
    /// The candidate sensor device list
    /// </param>
    /// <returns>The device change type</returns>
    ChangeFlag selectSensorFromCollection(const QList<INuiSensor *> &sensors);

    /// Update the sensor instance and its status, return if sensor and/or status is changed

    /// <param name="pNuiSensor">
    /// The new selected sensor instance
    /// </param>
    /// <param name="dwNewStatus">
    /// The status of the new selected sensor
    /// </param>
    /// <returns>The device change type</returns>
    ChangeFlag updateSensorAndStatus(INuiSensor *nuiSensor, const quint32 newStatus);

    /// Helper to update the required sensor id

    /// <param name="strRequiredSensorId">
    /// The required sensor Id
    /// </param>
    void setRequiredId(const QString &requiredSensorId);

    /// Check whether we need a specific sensor

    /// <returns>Return true if the sensor is required, else return false</returns>
    bool hasRequiredSensorId() const { return !m_requiredSensorId.isEmpty(); }

    /// Check if current sensor meets the user requirement

    /// <returns>Return true if current sensor meets user requirement</returns>
    bool isCurrentSensorRequired() const;

    /// Check if current sensor is in operateable state and meets the sensor id requirement

    /// <returns>Return true if current sensor is valid</returns>
    bool isCurrentSensorValid() const;

private:
    INuiSensor *m_nuiSensor;
    bool m_isSensorInitialized;
    quint32 m_latestStatus;
    QString m_requiredSensorId;
};

#endif // NUISENSORSELECTOR_H
