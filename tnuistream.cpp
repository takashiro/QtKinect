#include "tnuistream.h"
#include "tnuisensor.h"

TNuiStreamInternal::TNuiStreamInternal(TNuiSensor *sensor, QObject *parent)
    : QThread(parent)
    , m_sensor(sensor)
    , m_paused(false)
    , m_isOpen(false)
{
    m_frameReadyEvent = INVALID_HANDLE_VALUE;
    m_stopThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    connect(m_sensor, &TNuiSensor::connected, this, &TNuiStreamInternal::tryOpen);
}

TNuiStreamInternal::~TNuiStreamInternal()
{
    stop();
    CloseHandle(m_stopThreadEvent);
}

void TNuiStreamInternal::tryOpen()
{
    if (open()) {
        TNuiSensor *sensor = qobject_cast<TNuiSensor *>(sender());
        if (sensor)
            disconnect(sensor, &TNuiSensor::connected, this, &TNuiStreamInternal::tryOpen);
    }
}

void TNuiStreamInternal::stop()
{
    SetEvent(m_stopThreadEvent);
    wait();
}

void TNuiStreamInternal::run()
{
    if (m_frameReadyEvent == INVALID_HANDLE_VALUE) {
        qDebug("frame-ready event is not set.");
        return;
    }

    HANDLE events[] = {m_frameReadyEvent, m_stopThreadEvent};

    forever {
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

TNuiStream::TNuiStream(TNuiSensor *sensor)
    : QObject(sensor)
{
}

void TNuiStream::setInternal(TNuiStreamInternal *internal)
{
    d = internal;
    d->ref.ref();
    connect(d, &TNuiStreamInternal::readyRead, this, &TNuiStream::readyRead);
}

TNuiStream::~TNuiStream()
{
    d->ref.deref();
    if (d->ref.load() == 1) {
        delete d;
        d = nullptr;
    }
}

void TNuiStream::tryOpen()
{
    d->tryOpen();
}
