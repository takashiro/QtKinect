#ifndef TNUISTREAM_H
#define TNUISTREAM_H

#include <QThread>

#include <Windows.h>
#include <NuiApi.h>

class TNuiSensor;

class TNuiStreamInternal : public QThread
{
    Q_OBJECT

    friend class TNuiStream;

public:
    TNuiStreamInternal(TNuiSensor *sensor, QObject *parent = 0);
    ~TNuiStreamInternal();

    virtual bool open() = 0;
    virtual bool close() = 0;
    void tryOpen();

    void stop();

signals:
    void readyRead();

protected:
    void run();
    virtual bool processNewFrame() = 0;

    HANDLE m_frameReadyEvent;
    TNuiSensor *m_sensor;
    bool m_paused;
    bool m_isOpen;

private:
    HANDLE m_stopThreadEvent;
    QAtomicInt ref;
};

class TNuiStream : public QObject
{
    Q_OBJECT

public:
    TNuiStream(TNuiSensor *sensor);
    ~TNuiStream();

    TNuiSensor *sensor() const { return d->m_sensor; }

    bool open() { return d->open(); }
    bool close() { return d->close(); }
    bool isOpen() const { return d->m_isOpen; }
    void tryOpen();

signals:
    void readyRead();

protected:
    //The derived class is responsible to call this in it's constructor
    void setInternal(TNuiStreamInternal *internal);

private:
    TNuiStreamInternal *d;
};

#endif // TNUISTREAM_H
