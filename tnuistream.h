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
    TNuiStream(TNuiSensor *parent);
    virtual ~TNuiStream();

    TNuiSensor *sensor() const {return m_sensor;}

    virtual bool open() = 0;
    bool isOpen() const {return m_isOpen;}
    void tryOpen();
    void stop();

signals:
    void readyRead();

protected:
    virtual bool processNewFrame() = 0;
    void run();

    HANDLE m_frameReadyEvent;
    TNuiSensor *m_sensor;
    bool m_paused;
    bool m_isOpen;

private:
    HANDLE m_stopThreadEvent;
};

#endif // TNUISTREAM_H
