#include "tnuistream.h"
#include "tnuisensor.h"

TNuiStream::TNuiStream(TNuiSensor *parent)
    : QThread(parent)
    , m_sensor(parent)
    , m_paused(false)
    , m_isOpen(false)
{
    m_frameReadyEvent = INVALID_HANDLE_VALUE;
    m_stopThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    connect(m_sensor, &TNuiSensor::connected, this, &TNuiStream::tryOpen);
}

TNuiStream::~TNuiStream()
{
    stop();
    CloseHandle(m_stopThreadEvent);
}

void TNuiStream::tryOpen()
{
    if (open()) {
        TNuiSensor *sensor = qobject_cast<TNuiSensor *>(sender());
        if (sensor)
            disconnect(sensor, &TNuiSensor::connected, this, &TNuiStream::tryOpen);
    }
}

void TNuiStream::pause(bool pause)
{
    if (m_paused == pause)
        return;

    m_paused = pause;
    if (pause)
        m_mutex.lock();
    else
        m_mutex.unlock();
}

void TNuiStream::stop()
{
    SetEvent(m_stopThreadEvent);
    pause(false);
    wait();
}

void TNuiStream::run()
{
    if (m_frameReadyEvent == INVALID_HANDLE_VALUE) {
        qDebug("frame-ready event is not set.");
        return;
    }

    HANDLE events[] = {m_frameReadyEvent, m_stopThreadEvent};

    forever {
        m_mutex.lock();
        m_mutex.unlock();
        DWORD ret = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);
        ret -= WAIT_OBJECT_0;
        if (ret == 0) {//m_frameReadyEvent
            if (processNewFrame())
                emit readyRead();
        } else if (ret == 1) {//m_stopThreadEvent
            break;
        } else {
            qDebug("unexpected event");
            break;
        }
    }
}
