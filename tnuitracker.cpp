#include "tnuitracker.h"
#include "tnuiskeletonstream.h"
#include "tnuiimagestream.h"
#include "tnuisensor.h"

#include <QPointF>

QPointF MapToScreen(const Vector4 &point)
{
    //@to-do: It's not fixed
    const NUI_IMAGE_RESOLUTION resolution = NUI_IMAGE_RESOLUTION_640x480;
    long x, y;
    ushort depth;

    NuiTransformSkeletonToDepthImage(point, &x, &y, &depth, resolution); // Returns coordinates in depth space

    long backupX = x, backupY = y;
    if (FAILED(NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(resolution, resolution, NULL, x, y, depth, &x, &y))) {
        x = backupX;
        y = backupY;
    }

    return QPointF(x, y);
}

TNuiSkeletonStream *TNuiTracker::m_skeletonStream;

TNuiTracker::TNuiTracker(TNuiSensor *sensor, NUI_SKELETON_POSITION_INDEX target)
    : QObject(sensor)
    , m_target(target)
{
    static TNuiSkeletonStream *stream = m_skeletonStream = new TNuiSkeletonStream(sensor);
    connect(stream, &TNuiSkeletonStream::readyRead, this, &TNuiTracker::handleNewFrame);
    stream->tryOpen();
}

void TNuiTracker::handleNewFrame()
{
    if (m_target == NUI_SKELETON_POSITION_COUNT)
        return;

    NUI_SKELETON_FRAME frame;
    m_skeletonStream->readFrame(frame);

    //@todo: Track more players
    for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
        if (frame.SkeletonData[i].dwTrackingID != 0) {
            const Vector4 &pos3d = frame.SkeletonData[i].SkeletonPositions[m_target];
            m_screenPos = MapToScreen(pos3d);
            emit moved(m_screenPos);
            m_z = pos3d.z;
            emit zChanged(m_z);
            break;
        }
    }
}
