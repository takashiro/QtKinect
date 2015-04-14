#include "tnuicolorcamera.h"
#include "tnuisensor.h"
#include "tnuisensormanager.h"
#include "tnuibackgroundremovedcolorstream.h"

#include <QQuickWindow>
#include <QSGSimpleTextureNode>

TNuiColorCamera::TNuiColorCamera(QQuickItem *parent)
    : QQuickItem(parent)
    , m_texture(nullptr)
{
    setFlag(ItemHasContents, true);

    TNuiSensor *sensor = SensorManager->sensor();
    if (sensor != nullptr) {
        m_stream = new TNuiColorStream(sensor);
        connect(m_stream, &TNuiColorStream::readyRead, this, &TNuiColorCamera::updateFrame);
        m_stream->tryOpen();
    }
}

TNuiColorCamera::~TNuiColorCamera()
{
    delete m_texture;
}

bool TNuiColorCamera::backgroundRemoved() const
{
    return m_stream->inherits("TNuiBackgroundRemovedColorStream");
}

void TNuiColorCamera::setBackgroundRemoved(bool removed)
{
    TNuiSensor *sensor = SensorManager->sensor();
    if (sensor == nullptr)
        return;

    if (removed != backgroundRemoved()) {
        delete m_stream;
        m_stream = removed ? new TNuiBackgroundRemovedColorStream(sensor) : new TNuiColorStream(sensor);
        connect(m_stream, &TNuiColorStream::readyRead, this, &TNuiColorCamera::updateFrame);
        m_stream->tryOpen();
    }
}

void TNuiColorCamera::updateFrame()
{
    m_image = m_stream->readImage();
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
