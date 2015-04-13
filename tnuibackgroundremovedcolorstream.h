#ifndef TNUIBACKGROUNDREMOVEDCOLORSTREAM_H
#define TNUIBACKGROUNDREMOVEDCOLORSTREAM_H

#include "tnuicolorstream.h"

class TNuiDepthStream;
class TNuiSkeletonStream;

class TNuiBackgroundRemovedColorStreamPrivate;
class TNuiBackgroundRemovedColorStream : public TNuiColorStream
{
    Q_OBJECT

    friend class TNuiBackgroundRemovedColorStreamPrivate;

public:
    TNuiBackgroundRemovedColorStream(TNuiSensor *parent);
    ~TNuiBackgroundRemovedColorStream();

    bool open();
    bool close();

protected:
    bool processNewFrame();
    void processDepth();
    void processSkeleton();

    bool chooseSkeleton(NUI_SKELETON_DATA *skeletonData);

    TNuiDepthStream *m_depthStream;
    TNuiSkeletonStream *m_skeletonStream;
    TNuiBackgroundRemovedColorStreamPrivate *p_ptr;

    DWORD m_trackedSkeleton;

    uchar *m_colorData;
    QMutex m_colorDataMutex;

private:
    Q_DISABLE_COPY(TNuiBackgroundRemovedColorStream)
};

#endif // TNUIBACKGROUNDREMOVEDCOLORSTREAM_H
