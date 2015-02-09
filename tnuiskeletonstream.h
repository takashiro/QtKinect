#ifndef TNUISKELETONSTREAM_H
#define TNUISKELETONSTREAM_H

#include "tnuistream.h"

class TNuiSensor;

class TNuiSkeletonStream : public TNuiStream
{
public:
    enum TrackingFlag{
        SupressNoFrameData = NUI_SKELETON_TRACKING_FLAG_SUPPRESS_NO_FRAME_DATA,
        TitleSetsTrackedSkeletions = NUI_SKELETON_TRACKING_FLAG_TITLE_SETS_TRACKED_SKELETONS,
        EnableSeatedSupport = NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT,
        EnableInNearRange = NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE
    };
    typedef ulong TrackingFlags;

    TNuiSkeletonStream(TNuiSensor *sensor, TrackingFlags flags);

    bool open();
    bool close();

protected:
    bool processNewFrame();
    void readFrame(NUI_SKELETON_FRAME &frame);

    ulong m_flags;
    NUI_SKELETON_FRAME m_frame;
    QMutex m_frameMutex;
};

#endif // TNUISKELETONSTREAM_H
