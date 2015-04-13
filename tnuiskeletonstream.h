#ifndef TNUISKELETONSTREAM_H
#define TNUISKELETONSTREAM_H

#include "tnuistream.h"

class TNuiSensor;

class TNuiSkeletonStreamPrivate;

class TNuiSkeletonStream : public QObject
{
    Q_OBJECT

public:
    friend class TNuiSensor;

    enum TrackingFlag{
        SupressNoFrameData = NUI_SKELETON_TRACKING_FLAG_SUPPRESS_NO_FRAME_DATA,
        TitleSetsTrackedSkeletions = NUI_SKELETON_TRACKING_FLAG_TITLE_SETS_TRACKED_SKELETONS,
        EnableSeatedSupport = NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT,
        EnableInNearRange = NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE
    };
    typedef ulong TrackingFlags;

    TNuiSkeletonStream(TNuiSensor *sensor, TrackingFlags flags = EnableInNearRange | EnableSeatedSupport);
    ~TNuiSkeletonStream();

    bool open();
    void tryOpen();
    bool close();
    bool reopen();

    void setFlags(TrackingFlags flags);
    void resetFlags(TrackingFlags flags);
    TrackingFlags flags() const;

    void readFrame(NUI_SKELETON_FRAME &frame);

signals:
    void readyRead();

protected:
    static TNuiSkeletonStreamPrivate *p_ptr;
};

class TNuiSkeletonStreamPrivate: public TNuiStream
{
   Q_OBJECT

   friend class TNuiSkeletonStream;

protected:
   TNuiSkeletonStreamPrivate(TNuiSensor *sensor);
   ~TNuiSkeletonStreamPrivate();

   bool open();
   bool processNewFrame();

   TNuiSkeletonStream::TrackingFlags flags;
   NUI_SKELETON_FRAME frame;
   QMutex frameMutex;
   QAtomicInt ref;
};

#endif // TNUISKELETONSTREAM_H
