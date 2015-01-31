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
    QVector<uchar> data() const {return m_data;}

    bool open();
    bool isOpen() const {return m_isOpen;}

    // Pause the stream
    void pause(bool pause);

signals:
    void readyRead();

protected:
    virtual void processNewFrame() = 0;

    HANDLE m_streamHandle;
    HANDLE m_frameReadyEvent;
    TNuiSensor *m_sensor;
    QVector<uchar> m_data;
    NUI_IMAGE_TYPE m_imageType;
    NUI_IMAGE_RESOLUTION m_imageResolution;
    bool m_paused;
    bool m_isOpen;

private:
    void run();

    QMutex m_mutex;
    HANDLE m_stopThreadEvent;
};

#endif // TNUISTREAM_H
