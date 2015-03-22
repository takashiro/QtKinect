#include "tnuicolorcamera.h"
#include "tnuisensor.h"
#include "tnuiimagestream.h"
#include "tnuisensormanager.h"

#include <QQuickWindow>
#include <QSGSimpleTextureNode>

TNuiColorCamera::TNuiColorCamera(QQuickItem *parent)
    : QQuickItem(parent)
    , m_image(640, 480, QImage::Format_RGB32)
    , m_texture(nullptr)
{
    setWidth(640);
    setHeight(480);
    m_image.fill(Qt::black);

    setFlag(ItemHasContents, true);

    TNuiSensor *sensor = SensorManager->sensor();
    m_stream = sensor->createImageStream(TNuiImageStream::ColorType);
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

    delete m_texture;
    m_texture = window()->createTextureFromImage(m_image);
    update();
}

QSGNode *TNuiColorCamera::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    QSGSimpleTextureNode *textureNode = static_cast<QSGSimpleTextureNode *>(node);
    if (textureNode == nullptr)
        textureNode = new QSGSimpleTextureNode;
    textureNode->setRect(boundingRect());
    textureNode->setTexture(m_texture);
    return textureNode;
}
