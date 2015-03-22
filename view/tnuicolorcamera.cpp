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
    setFlag(ItemHasContents, true);
    m_image.fill(Qt::black);

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
    m_image = m_stream->readFrameImage();
    QQuickWindow *mainWindow = window();
    if (mainWindow) {
        delete m_texture;
        m_texture = mainWindow->createTextureFromImage(m_image);
        update();
    }
}

QSGNode *TNuiColorCamera::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    if (m_texture == nullptr)
        return node;

    QSGSimpleTextureNode *textureNode = static_cast<QSGSimpleTextureNode *>(node);
    if (!textureNode)
        textureNode = new QSGSimpleTextureNode;
    textureNode->setRect(boundingRect());
    textureNode->setTexture(m_texture);
    return textureNode;
}
