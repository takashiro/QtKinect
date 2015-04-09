#include "tnuitracker.h"
#include "tnuiskeletonstream.h"
#include "tnuiimagestream.h"
#include "tnuisensor.h"

QPointF MapToScreen(const Vector4 &point, TNuiImageStream *stream) {
    const NUI_IMAGE_RESOLUTION resolution = (NUI_IMAGE_RESOLUTION) stream->imageResolution();
    long x;
    long y;
    ushort depth;

    NuiTransformSkeletonToDepthImage(point, &x, &y, &depth, resolution); // Returns coordinates in depth space

    LONG backupX = x, backupY = y;
    if (FAILED(NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(resolution, resolution, &stream->frame().ViewArea, x, y, depth, &x, &y))) {
        x = backupX;
        y = backupY;
    }

    /*ulong imageWidth, imageHeight;
    NuiImageResolutionToSize(imageResolution, imageWidth, imageHeight);

    float resultX, resultY;
    resultX = x * (imageRect.right  - imageRect.left + 1.0f) / imageWidth + imageRect.left;
    resultY = y * (imageRect.bottom - imageRect.top  + 1.0f) / imageHeight + imageRect.top;*/

    return QPointF(x, y);
}

TNuiTracker::TNuiTracker(TNuiSensor *sensor, NUI_SKELETON_POSITION_INDEX target)
    : QObject(sensor)
    , m_skeletonStream(new TNuiSkeletonStream(sensor))
    , m_colorStream(new TNuiColorStream(sensor))
    , m_target(target)
{
    connect(m_skeletonStream, &TNuiSkeletonStream::readyRead, this, &TNuiTracker::handleNewFrame);
    m_skeletonStream->open();
}

void TNuiTracker::handleNewFrame()
{
    if (m_target == NUI_SKELETON_POSITION_COUNT)
        return;

    NUI_SKELETON_FRAME frame;
    m_skeletonStream->readFrame(frame);

    //@todo: Track more players
    for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
        if (frame.SkeletonData[i].dwTrackingID != 0){
            QPointF pos = MapToScreen(frame.SkeletonData[i].SkeletonPositions[m_target], m_colorStream);
            emit moved(pos);
            break;
        }
    }
}
