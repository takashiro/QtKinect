#include "tnuisensorselector.h"

#include "windowsutil.h"

#include <Windows.h>
#include <NuiApi.h>


// Check whether the specified sensor is available.
bool IsSensorConflict(INuiSensor *nuiSensor)
{
    Q_ASSERT(nuiSensor != nullptr);

    // Because we can still open a sensor even if it is occupied by other process,
    // we have to explicitly initialize it to check if we can operate it.
    HRESULT hr = nuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR);

    if (SUCCEEDED(hr)) {
        nuiSensor->NuiShutdown();
        return false;
    }

    return true;
}

// Return the current status of the sensor
TNuiSensorSelector::Status GetStatusFromSensor(INuiSensor *nuiSensor, bool checkConflict = false)
{
    Q_ASSERT(nuiSensor != nullptr);

    TNuiSensorSelector::Status curStatus;

    HRESULT hr = nuiSensor->NuiStatus();
    switch (hr)
    {
    case S_OK:
        curStatus = TNuiSensorSelector::StartedStatus;

        if (checkConflict)
        {
            // Even if the input sensor is started, we still need to check if it is
            // occupied by other process.
            if (IsSensorConflict(nuiSensor))
            {
                // other app owns this sensor
                curStatus = TNuiSensorSelector::ConflictStatus;
            }
        }
        break;
    case S_NUI_INITIALIZING:
        curStatus = TNuiSensorSelector::InitializingStatus;
        break;
    case E_NUI_NOTGENUINE:
        curStatus = TNuiSensorSelector::NotGenuineStatus;
        break;
    case E_NUI_INSUFFICIENTBANDWIDTH:
        curStatus = TNuiSensorSelector::InsufficientBandwidthStatus;
        break;
    case E_NUI_NOTSUPPORTED:
        curStatus = TNuiSensorSelector::NotSupportedStatus;
        break;
    case E_NUI_NOTPOWERED:
        curStatus = TNuiSensorSelector::NotPoweredStatus;
        break;
    default:
        curStatus = TNuiSensorSelector::ErrorStatus;
    }

    return curStatus;
}


TNuiSensorSelector::TNuiSensorSelector()
    : m_nuiSensor(nullptr)
    , m_latestStatus(NoneStatus)
    , m_isSensorInitialized(false)
{
    tryFindNuiSensor();
}

TNuiSensorSelector::~TNuiSensorSelector()
{
    updateSensorAndStatus(nullptr, NoneChangedFlag);
}

TNuiSensorSelector::ChangeFlag TNuiSensorSelector::onNuiStatusChanged()
{
    return tryFindNuiSensor();
}

INuiSensor *TNuiSensorSelector::getSensor(quint32 flags)
{
    if (nullptr == m_nuiSensor)
    {
        return nullptr;
    }

    if (m_nuiSensor->NuiInitializationFlags() != flags)
    {
        // If the input init flag is different from current init flags in the sensor,
        // we will re-initialize the sensor
        HRESULT hr = m_nuiSensor->NuiInitialize(flags);

        if (FAILED(hr))
        {
            // Set the return point to null and convert all
            return nullptr;
        }

        // Mark sensor is initialized
        m_isSensorInitialized = true;
    }

    m_nuiSensor->AddRef();

    return m_nuiSensor;
}

quint32 TNuiSensorSelector::getStatus() const
{
    return m_latestStatus;
}

TNuiSensorSelector::ChangeFlag TNuiSensorSelector::tryResolveConflict()
{
    TNuiSensorSelector::ChangeFlag flag = NoneChangedFlag;

    if ((m_latestStatus & ConflictStatus) != 0)
    {
        // Retry to find a sensor
        flag = tryFindNuiSensor();
    }

    return flag;
}

TNuiSensorSelector::ChangeFlag TNuiSensorSelector::setDesiredSensor(const QString &requiredSensorId)
{
    setRequiredId(requiredSensorId);

    // We either have no sensor or the sensor we have isn't the one they asked for
    return tryFindNuiSensor();
}

TNuiSensorSelector::ChangeFlag TNuiSensorSelector::tryFindNuiSensor()
{
    if (isCurrentSensorValid())
    {
        return NoneChangedFlag;
    }

    // Check if user want to find a specific sensor
    if (hasRequiredSensorId()) {
        return tryFindRequiredNuiSensor();
    } else {
        return tryFindAnyOneNuiSensor();
    }
}

bool TNuiSensorSelector::isCurrentSensorValid() const
{
    if (nullptr == m_nuiSensor)
    {
        return false;
    }

    // Get current sensor status. Sinece we already own this
    // sensor, we do not need to check the conflict status.
    Status curStatus = GetStatusFromSensor(m_nuiSensor, false);

    if (curStatus == StartedStatus) {
        // If current sensor is started, we then check if it is the required
        // sensor specified by users
        return !hasRequiredSensorId() || isCurrentSensorRequired();
    }

    return false;
}

TNuiSensorSelector::ChangeFlag TNuiSensorSelector::tryFindRequiredNuiSensor()
{
    Q_ASSERT(!m_requiredSensorId.empty());

    INuiSensor *nuiSensor = nullptr;
    HRESULT hr = NuiCreateSensorById(WindowsUtil::ToOLECharArray(m_requiredSensorId), &nuiSensor);

    QList<INuiSensor *> sensors;

    if (SUCCEEDED(hr))
    {
        sensors << nuiSensor;
    }

    return selectSensorFromCollection(sensors);
}

TNuiSensorSelector::ChangeFlag TNuiSensorSelector::tryFindAnyOneNuiSensor()
{
    Q_ASSERT(m_requiredSensorId.empty());

    QList<INuiSensor *> sensors;

    int iSensorCount = 0;
    HRESULT hr = NuiGetSensorCount(&iSensorCount);

    if (SUCCEEDED(hr))
    {
        for (int i = 0; i < iSensorCount; ++i)
        {
            INuiSensor* nuiSensor = nullptr;
            HRESULT hr = NuiCreateSensorByIndex(i, &nuiSensor);

            if (SUCCEEDED(hr))
            {
                sensors << nuiSensor;
            }
        }
    }

    return selectSensorFromCollection(sensors);
}

TNuiSensorSelector::ChangeFlag TNuiSensorSelector::selectSensorFromCollection(const QList<INuiSensor *> &sensors)
{
    // The status bits are used to save all the error status collected from each sensor.
    // At first, we mark the error bits as NoAvailableSensors if the sensor collection is empty,
    // or just set to None.
    quint32 statusBits = sensors.size() == 0 ? NoAvailableSensorsStatus : NoneStatus;
    INuiSensor *validSensor = nullptr;

    foreach (INuiSensor *sensor, sensors) {
        // Check if current sensor is started and has no conflict
        Status status = GetStatusFromSensor(sensor, true);

        if (StartedStatus == status)
        {
            // Since sensor is started and no conflict, we treat it as a valid sensor.
            validSensor = sensor;
            // Reset the status bits to Started state since we find a valid sensor.
            statusBits = StartedStatus;
            break;
        }

        // Combine the sensor error status to the error bits
        statusBits |= status;
    }

    // Release all the interfaces except the valid one.
    foreach (INuiSensor *sensor, sensors)
    {
        if (sensor != validSensor)
        {
            // Release the sensor instance if it is not operateable
            sensor->Release();
        }
    }

    // Update the internal sensor instance and status. We have
    // two combinations:
    //     1. valid sensor interface pointer, StartedStatus
    //     2. nullptr, Error bits combination
    return updateSensorAndStatus(validSensor, statusBits);
}

TNuiSensorSelector::ChangeFlag TNuiSensorSelector::updateSensorAndStatus(INuiSensor* nuiSensor, const quint32 newStatus)
{
    ChangeFlag changedFlags = NoneChangedFlag;

    if (newStatus != m_latestStatus)
    {
        m_latestStatus = newStatus;
        changedFlags = StatusChangedFlag;
    }

    if (m_nuiSensor != nuiSensor)
    {
        // Release the old sensor instance first
        if (nullptr != m_nuiSensor)
        {
            if (m_isSensorInitialized)
            {
                // We should shutdown the sensor if we initialized it
                m_nuiSensor->NuiShutdown();
            }

            m_nuiSensor->Release();
        }

        // Set current sensor to the new one and reset the init flag to not initialized
        m_nuiSensor = nuiSensor;
        m_isSensorInitialized = false;

        changedFlags = SensorChangedFlag;
    }

    return changedFlags;
}

bool TNuiSensorSelector::isCurrentSensorRequired() const
{
    Q_ASSERT(m_nuiSensor != nullptr);

    WCHAR *sensorId = m_nuiSensor->NuiDeviceConnectionId();

    if (nullptr == sensorId) {
        // If the required d is not empty, the null device id of current sensor
        // cannot match it, so we should return false for this case.
        // If the required id is empty, it means any sensor is ok, so we will
        // return true even if the current sensor id is null.
        return m_requiredSensorId.isEmpty();
    }

    // case sensitive comparison
    return m_requiredSensorId == sensorId;
}

void TNuiSensorSelector::setRequiredId(const QString &requiredSensorId)
{
    if (requiredSensorId != nullptr) {
        m_requiredSensorId = requiredSensorId;
    } else {
        // Since user do not specify a sensor id, we will clear the existing sensor id
        // to indicate that any sensor is acceptable
        m_requiredSensorId.clear();
    }
}
