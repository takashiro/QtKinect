#include "tnuicolorcamera.h"
#include "tnuisensor.h"
#include "tnuiimagestream.h"
#include "tnuisensormanager.h"

TNuiColorCamera::TNuiColorCamera(QQuickItem *parent)
    : TImage(parent)
{
    m_image = QImage(640, 480, QImage::Format_RGB32);
    m_image.fill(Qt::black);

    TNuiSensor *sensor = SensorManager->sensor();
    m_stream = new TNuiColorStream(sensor);
    connect(sensor, &TNuiSensor::stateChanged, this, &TNuiColorCamera::tryOpenStream);
    tryOpenStream();
}

void TNuiColorCamera::tryOpenStream()
{
    if (m_stream->open()) {
        disconnect(m_stream->sensor(), &TNuiSensor::stateChanged, this, &TNuiColorCamera::tryOpenStream);
        connect(m_stream, &TNuiStream::readyRead, this, &TNuiColorCamera::updateFrame);
    }
}

void TNuiColorCamera::updateFrame()
{
    int color;
    uchar *rgba = reinterpret_cast<uchar *>(&color);
    QVector<uchar> data = m_stream->data();
    int k = 0;
    for (int j = 0; j < 480; j++) {
        for (int i = 0; i < 640; i++) {
            rgba[0] = data.at(k++);
            rgba[1] = data.at(k++);
            rgba[2] = data.at(k++);
            rgba[3] = data.at(k++);
            m_image.setPixel(i, j, color);
        }
    }
    update();
}
