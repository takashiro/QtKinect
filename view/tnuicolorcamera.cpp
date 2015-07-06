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
    , m_backgroundRemovalEffect(nullptr)
    , m_texture(nullptr)
{
    setFlag(ItemHasContents, true);

    TNuiSensor *sensor = SensorManager->sensor();
    if (sensor != nullptr) {
        m_stream = new TNuiColorStream(sensor);
        connect(m_stream, &TNuiColorStream::readyRead, this, &TNuiColorCamera::update);
        m_stream->tryOpen();
    }
}

TNuiColorCamera::~TNuiColorCamera()
{
    delete m_backgroundRemovalEffect;
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

QSGNode *TNuiColorCamera::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    QQuickWindow *mainWindow = window();
    if (!mainWindow)
        return node;

    QSGSimpleTextureNode *textureNode = static_cast<QSGSimpleTextureNode *>(node);
    if (!textureNode)
        textureNode = new QSGSimpleTextureNode;

    if (m_texture)
        m_texture->deleteLater();

    QImage image(m_stream->readImage());
    m_texture = mainWindow->createTextureFromImage(image);
    textureNode->setTexture(m_texture);
    textureNode->setRect(boundingRect());

    return textureNode;
}
