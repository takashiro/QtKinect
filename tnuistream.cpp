#include "stdafx.h"
#include "tnuistream.h"
#include "tnuisensor.h"

TNuiStream::TNuiStream(TNuiSensor *parent)
    : QThread(parent)
    , m_sensor(parent)
    , m_frameReadyEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr))
    , m_paused(false)
    , m_isOpen(false)
    , m_stopThreadEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr))
{
}

TNuiStream::~TNuiStream()
{
    SetEvent(m_stopThreadEvent);
    pause(false);
    wait();
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

void TNuiStream::run()
{
    HANDLE events[] = {m_stopThreadEvent, m_frameReadyEvent};

    forever {
        m_mutex.lock();
        m_mutex.unlock();
        DWORD ret = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);
        ret -= WAIT_OBJECT_0;
        if (ret == 0){//m_stopThreadEvent
            break;
        } else if (ret == 1) {//m_frameReadyEvent
            if (processNewFrame())
                emit readyRead();
        } else {
            break;
        }
    }
}
