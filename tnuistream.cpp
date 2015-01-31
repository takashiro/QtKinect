#include "stdafx.h"
#include "tnuistream.h"
#include "tnuisensor.h"

TNuiStream::TNuiStream(TNuiSensor *parent)
    : QThread(parent)
    , m_sensor(parent)
    , m_streamHandle(INVALID_HANDLE_VALUE)
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

bool TNuiStream::open()
{
    m_isOpen = m_sensor->openImageStream(this, 0, 2);
    return m_isOpen;
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
        DWORD ret = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);
        if (WAIT_OBJECT_0 == ret){
            break;
        } else if (WAIT_OBJECT_0 + 1 == ret) {
            processNewFrame();
            emit readyRead();
        }
        m_mutex.unlock();
    }
}
