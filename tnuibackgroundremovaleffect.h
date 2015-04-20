#ifndef TNUIBACKGROUNDREMOVEDCOLORSTREAM_H
#define TNUIBACKGROUNDREMOVEDCOLORSTREAM_H

#ifdef KINECT_USE_BACKGROUNDREMOVAL

#include <Windows.h>
#include <NuiApi.h>

#include "tnuicolorstreameffect.h"

#include <QThread>
#include <QReadWriteLock>

class TNuiColorStream;
class TNuiDepthStream;
class TNuiSkeletonStream;
class TNuiBackgroundRemovalThread;

struct INuiBackgroundRemovedColorStream;

class TNuiBackgroundRemovalEffect : public TNuiColorStreamEffect
{
    Q_OBJECT

    friend class TNuiBackgroundRemovalThread;

public:
    TNuiBackgroundRemovalEffect(TNuiColorStream *stream);
    ~TNuiBackgroundRemovalEffect();

protected:
    void updateFrameData(uchar *data, uint length);

    void processColor();
    void processDepth();
    void processSkeleton();

    bool chooseSkeleton(NUI_SKELETON_DATA *skeletonData);

    INuiBackgroundRemovedColorStream *m_stream;
    HANDLE m_frameReadyEvent;
    HANDLE m_stopEvent;
    DWORD m_trackedSkeleton;

    TNuiDepthStream *m_depthStream;
    TNuiSkeletonStream *m_skeletonStream;

    uchar *m_data;
    uint m_dataSize;
    QReadWriteLock m_dataMutex;

    TNuiBackgroundRemovalThread *m_thread;
};

#endif

#endif // TNUIBACKGROUNDREMOVEDCOLORSTREAM_H
