#ifndef TNUIINTERACTIONSTREAM_H
#define TNUIINTERACTIONSTREAM_H

#ifdef KINECT_USE_INTERACTION

#include "tnuistream.h"

#include <QReadWriteLock>

#include <KinectInteraction.h>

class TNuiSensor;
class TNuiSkeletonStream;
class TNuiDepthStream;

class TNuiInteractionStreamInternal;

class TNuiInteractionStream : public TNuiStream
{
    Q_OBJECT

public:
    TNuiInteractionStream(TNuiSensor *sensor);
    ~TNuiInteractionStream();

    void readFrame(NUI_INTERACTION_FRAME &frame);

    static QPointer<TNuiInteractionStreamInternal> d;
};

class TNuiInteractionStreamInternal : public TNuiStreamInternal
{
    Q_OBJECT

    friend class TNuiInteractionStream;

public:
    TNuiInteractionStreamInternal(TNuiSensor *sensor, QObject *parent = 0);
    ~TNuiInteractionStreamInternal();

    bool open();
    bool close();

protected:
    bool processNewFrame();

private:
    void processDepth();
    void processSkeleton();

protected:
    INuiInteractionStream *m_stream;
    INuiInteractionClient *m_interactionClient;
    TNuiSkeletonStream *m_skeletonStream;
    TNuiDepthStream *m_depthStream;

    NUI_INTERACTION_FRAME m_frame;
    QReadWriteLock m_frameMutex;
};

#endif

#endif // TNUIINTERACTIONSTREAM_H
