#include "tnuigesture.h"
#include "tnuisensormanager.h"
#include "tnuidepthstream.h"
#include "tnuiskeletonstream.h"

TNuiDepthStream *TNuiGesture::m_depthStream;
TNuiSkeletonStream *TNuiGesture::m_skeletonStream;

TNuiGesture::TNuiGesture(QObject *parent)
    : QObject(parent)
{
    TNuiSensor *sensor = SensorManager->sensor();

    static TNuiDepthStream *depthStream = m_depthStream = new TNuiDepthStream(sensor, true);
    connect(depthStream, &TNuiDepthStream::readyRead, this, &TNuiGesture::processDepth);

    static TNuiSkeletonStream *skeletonStream = m_skeletonStream = new TNuiSkeletonStream(sensor);
    connect(skeletonStream, &TNuiSkeletonStream::readyRead, this, &TNuiGesture::processSkeleton);
}

TNuiGesture::~TNuiGesture()
{

}

void TNuiGesture::processDepth()
{
}

void TNuiGesture::processSkeleton()
{
}
