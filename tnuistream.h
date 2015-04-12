#ifndef TNUISTREAM_H
#define TNUISTREAM_H

#include <QThread>
#include <QMutex>
#include <QVector>

#include <Windows.h>
#include <NuiApi.h>

class TNuiSensor;

class TNuiStream : public QThread
{
    Q_OBJECT

public:
    friend class TNuiSensor;

    TNuiStream(TNuiSensor *parent);
    virtual ~TNuiStream();

    TNuiSensor *sensor() const {return m_sensor;}

    virtual bool open() = 0;
    bool isOpen() const {return m_isOpen;}

    // Pause the stream
    void pause(bool pause);
    void stop();

signals:
    void readyRead();

protected:
    virtual bool processNewFrame() = 0;

    HANDLE m_frameReadyEvent;
    TNuiSensor *m_sensor;
    bool m_paused;
    bool m_isOpen;

private:
    void run();

    QMutex m_mutex;
    HANDLE m_stopThreadEvent;
};

#endif // TNUISTREAM_H
