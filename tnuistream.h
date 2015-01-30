#ifndef TNUISTREAM_H
#define TNUISTREAM_H

#include <QObject>

#include <Windows.h>
#include <NuiApi.h>

class TNuiStream : public QObject
{
    Q_OBJECT

public:
    TNuiStream(INuiSensor *nuiSensor);
    virtual ~TNuiStream();

public:
    // Subclass should override this method to process the next incoming
    // stream frame when stream event is set.
    virtual void processStreamFrame() = 0;

    // Pause the stream
    virtual void pauseStream(bool pause);

    // Subclass should override this method to start the stream processing.
    virtual bool startStream() = 0;

    HANDLE getFrameReadyEvent() {return m_frameReadyEvent;}

protected:
    INuiSensor *m_nuiSensor;

    bool m_paused;
    HANDLE m_streamHandle;
    HANDLE m_frameReadyEvent;
};

#endif // TNUISTREAM_H
