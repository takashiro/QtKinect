#include "tnuicolorcamera.h"
#include "tnuisensor.h"
#include "tnuisensormanager.h"
#include "tnuicolorstream.h"
#ifdef KINECT_USE_BACKGROUNDREMOVAL
#include "tnuibackgroundremovaleffect.h"
#endif

#include <QQuickWindow>
#include <QSGSimpleTextureNode>

TNuiColorCamera::TNuiColorCamera(QQuickItem *parent)
    : QQuickItem(parent)
    , m_texture(nullptr)
    , m_backgroundRemovalEffect(nullptr)
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
    return m_backgroundRemovalEffect != nullptr && m_stream->hasEffect(m_backgroundRemovalEffect);
}

void TNuiColorCamera::setBackgroundRemoved(bool removed)
{
#ifdef KINECT_USE_BACKGROUNDREMOVAL
    if (removed != backgroundRemoved()) {
        if (removed) {
            m_backgroundRemovalEffect = new TNuiBackgroundRemovalEffect(m_stream);
            m_stream->addEffect(m_backgroundRemovalEffect);
        } else {
            m_stream->removeEffect(m_backgroundRemovalEffect);
            delete m_backgroundRemovalEffect;
            m_backgroundRemovalEffect = nullptr;
        }
    }
#else
    Q_UNUSED(removed);
#endif
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
