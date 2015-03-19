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

    TNuiImageStream::ImageType type = stream->imageType();
    if (TNuiImageStream::ColorType == type
        || TNuiImageStream::ColorInfraredType == type
        || TNuiImageStream::ColorRawBayerType == type
        || TNuiImageStream::ColorRawYUVType == type
        || TNuiImageStream::ColorYUVType == type) {
        LONG backupX = x, backupY = y;
        if (FAILED(NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(resolution, resolution, &stream->frame().ViewArea, x, y, depth, &x, &y)))
        {
            x = backupX;
            y = backupY;
        }
    }

    /*ulong imageWidth, imageHeight;
    NuiImageResolutionToSize(imageResolution, imageWidth, imageHeight);

    float resultX, resultY;
    resultX = x * (imageRect.right  - imageRect.left + 1.0f) / imageWidth + imageRect.left;
    resultY = y * (imageRect.bottom - imageRect.top  + 1.0f) / imageHeight + imageRect.top;*/

    return QPointF(x, y);
}

TNuiTracker::TNuiTracker(TNuiSensor *sensor, TNuiImageStream *imageStream)
    : QObject(sensor)
    , m_skeletonStream(sensor->createSkeletonStream())
    , m_imageStream(imageStream)
{
    connect(m_skeletonStream, &TNuiSkeletonStream::readyRead, this, &TNuiTracker::handleNewFrame);
}

void TNuiTracker::handleNewFrame()
{
    NUI_SKELETON_FRAME frame;
    m_skeletonStream->readFrame(frame);

    //@todo: Track more players
    for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
        if (frame.SkeletonData[i].dwTrackingID != 0){
            QPointF pos = MapToScreen(frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT], m_imageStream);
            emit moved(pos);
            break;
        }
    }
}
