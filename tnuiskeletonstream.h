#ifndef TNUISKELETONSTREAM_H
#define TNUISKELETONSTREAM_H

#include "tnuistream.h"

#include <QReadWriteLock>
#include <QPointer>

class TNuiSensor;

class TNuiSkeletonStreamInternal;

class TNuiSkeletonStream : public TNuiStream
{
    Q_OBJECT

public:
    enum TrackingFlag{
        SupressNoFrameData = NUI_SKELETON_TRACKING_FLAG_SUPPRESS_NO_FRAME_DATA,
        TitleSetsTrackedSkeletions = NUI_SKELETON_TRACKING_FLAG_TITLE_SETS_TRACKED_SKELETONS,
        EnableSeatedSupport = NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT,
        EnableInNearRange = NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE
    };
    typedef ulong TrackingFlags;

    TNuiSkeletonStream(TNuiSensor *sensor, TrackingFlags flags = EnableInNearRange | EnableSeatedSupport);
    ~TNuiSkeletonStream();

    void setFlags(TrackingFlags flags);
    void resetFlags(TrackingFlags flags);
    TrackingFlags flags() const;

    void readFrame(NUI_SKELETON_FRAME &frame);

signals:
    void readyRead();

protected:
    static QPointer<TNuiSkeletonStreamInternal> d;
};

class TNuiSkeletonStreamInternal: public TNuiStreamInternal
{
   Q_OBJECT

   friend class TNuiSkeletonStream;

protected:
   TNuiSkeletonStreamInternal(TNuiSensor *sensor);
   ~TNuiSkeletonStreamInternal();

   bool open();
   bool close();
   bool processNewFrame();

   TNuiSkeletonStream::TrackingFlags flags;
   NUI_SKELETON_FRAME frame;
   QReadWriteLock frameMutex;
   QAtomicInt ref;
};

#endif // TNUISKELETONSTREAM_H
