#ifndef TNUIINTERACTIONSTREAM_H
#define TNUIINTERACTIONSTREAM_H

#include "tnuistream.h"

#include <QReadWriteLock>

#include <KinectInteraction.h>

class TNuiSensor;
class TNuiSkeletonStream;
class TNuiDepthStream;

class TNuiInteractionStream : public TNuiStream
{
    Q_OBJECT

public:
    TNuiInteractionStream(TNuiSensor *sensor);
    ~TNuiInteractionStream();

    bool open();
    bool close();

    void readFrame(NUI_INTERACTION_FRAME &frame);

protected:
    bool processNewFrame();

    INuiInteractionStream *m_stream;
    INuiInteractionClient *m_interactionClient;
    TNuiSkeletonStream *m_skeletonStream;
    TNuiDepthStream *m_depthStream;

    NUI_INTERACTION_FRAME m_frame;
    QReadWriteLock m_frameMutex;

private:
    void processDepth();
    void processSkeleton();
};

#endif // TNUIINTERACTIONSTREAM_H
