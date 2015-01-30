
#include "stdafx.h"
#include "tnuistream.h"

TNuiStream::TNuiStream(INuiSensor* nuiSensor)
    : m_nuiSensor(nuiSensor)
    , m_streamHandle(INVALID_HANDLE_VALUE)
    , m_paused(false)
{
    if (m_nuiSensor) {
        m_nuiSensor->AddRef();
    }

    m_frameReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
}

TNuiStream::~TNuiStream()
{
    SafeRelease(m_nuiSensor);
}

void TNuiStream::pauseStream(bool pause)
{
    m_paused = pause;
}
