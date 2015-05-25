#include "tnuibackgroundremovaleffect.h"
#include "tnuicolorstream.h"
#include "tnuidepthstream.h"
#include "tnuiskeletonstream.h"
#include "tnuisensor.h"

#include <QThread>

#include <KinectBackgroundRemoval.h>

class TNuiBackgroundRemovalThread : public QThread
{
public:
    TNuiBackgroundRemovalThread(TNuiBackgroundRemovalEffect *effect)
        : QThread(effect)
        , effect(effect)
    {
    }

protected:
    void run()
    {
        forever{
            HANDLE events[] = {effect->m_frameReadyEvent, effect->m_stopEvent};
            DWORD event = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);
            event -= WAIT_OBJECT_0;
            if (event == 0) {
                NUI_BACKGROUND_REMOVED_COLOR_FRAME frame;
                HRESULT hr = effect->m_stream->GetNextFrame(0, &frame);
                if (hr == S_OK) {
                    effect->m_dataMutex.lockForWrite();
                    memcpy_s(effect->m_data, effect->m_dataSize, frame.pBackgroundRemovedColorData, frame.backgroundRemovedColorDataLength);
                    effect->m_dataMutex.unlock();
                }
                effect->m_stream->ReleaseFrame(&frame);
            } else {
                break;
            }
        }
    }

    TNuiBackgroundRemovalEffect *effect;
};

TNuiBackgroundRemovalEffect::TNuiBackgroundRemovalEffect(TNuiColorStream *stream)
    : TNuiColorStreamEffect(stream)
{
    m_trackedSkeleton = NUI_SKELETON_INVALID_TRACKING_ID;

    TNuiSensor *sensor = stream->sensor();

    m_dataSize = m_colorStream->dataSize();
    m_data = new uchar[m_dataSize];

    m_depthStream = new TNuiDepthStream(sensor, true);
    m_depthStream->setFlag(TNuiDepthStream::EnableNearMode, true);

    m_skeletonStream = new TNuiSkeletonStream(sensor);

    m_stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_frameReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    NuiCreateBackgroundRemovedColorStream(sensor->nativeSensor(), &m_stream);

    m_thread = new TNuiBackgroundRemovalThread(this);

    NUI_IMAGE_RESOLUTION colorResolution = (NUI_IMAGE_RESOLUTION) m_colorStream->resolution();
    NUI_IMAGE_RESOLUTION depthResolution = (NUI_IMAGE_RESOLUTION) m_depthStream->resolution();
    if (S_OK == m_stream->Enable(colorResolution, depthResolution, m_frameReadyEvent)) {
        m_thread->start();

        connect(m_colorStream, &TNuiColorStream::readyRead, this, &TNuiBackgroundRemovalEffect::processColor);
        connect(m_depthStream, &TNuiDepthStream::readyRead, this, &TNuiBackgroundRemovalEffect::processDepth);
        connect(m_skeletonStream, &TNuiSkeletonStream::readyRead, this, &TNuiBackgroundRemovalEffect::processSkeleton);

        m_depthStream->tryOpen();
        m_skeletonStream->tryOpen();
    } else {
        qFatal("INuiBackgroundRemovedColorStream can't be enabled.");
    }
}

TNuiBackgroundRemovalEffect::~TNuiBackgroundRemovalEffect()
{
    SetEvent(m_stopEvent);
    m_thread->wait();
    m_stream->Release();

    m_depthStream->deleteLater();
    m_skeletonStream->deleteLater();

    m_dataMutex.lockForWrite();
    delete[] m_data;
    m_data = nullptr;
    m_dataMutex.unlock();

    CloseHandle(m_stopEvent);
    CloseHandle(m_frameReadyEvent);
}

void TNuiBackgroundRemovalEffect::updateFrameData(uchar *data, uint length)
{
    m_dataMutex.lockForRead();
    memcpy_s(data, length, m_data, m_dataSize);
    m_dataMutex.unlock();
}

void TNuiBackgroundRemovalEffect::processColor()
{
    NUI_IMAGE_FRAME frame;
    m_colorStream->readFrame(frame);
    m_colorStream->lockData();
    m_stream->ProcessColor(m_colorStream->dataSize(), m_colorStream->data(), frame.liTimeStamp);
    m_colorStream->unlockData();
}

void TNuiBackgroundRemovalEffect::processDepth()
{
    NUI_IMAGE_FRAME frame;
    m_depthStream->readFrame(frame);
    m_depthStream->lockData();
    m_stream->ProcessDepth(m_depthStream->dataSize(), m_depthStream->data(), frame.liTimeStamp);
    m_depthStream->unlockData();
}

void TNuiBackgroundRemovalEffect::processSkeleton()
{
    NUI_SKELETON_FRAME frame;
    m_skeletonStream->readFrame(frame);

    NUI_SKELETON_DATA *skeletonData = frame.SkeletonData;
    // Background Removal Stream requires us to specifically tell it what skeleton ID to use as the foreground
    if (!chooseSkeleton(skeletonData))
        return;

    m_stream->ProcessSkeleton(NUI_SKELETON_COUNT, skeletonData, frame.liTimeStamp);
}

bool TNuiBackgroundRemovalEffect::chooseSkeleton(NUI_SKELETON_DATA *skeletonData)
{
    // First we go through the stream to find the closest skeleton, and also check whether our current tracked
    // skeleton is still visibile in the stream
    float closestSkeletonDistance = FLT_MAX;
    DWORD closestSkeleton = NUI_SKELETON_INVALID_TRACKING_ID;
    BOOL isTrackedSkeletonVisible = false;
    for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
        NUI_SKELETON_DATA skeleton = skeletonData[i];
        if (NUI_SKELETON_TRACKED == skeleton.eTrackingState) {
            if (m_trackedSkeleton == skeleton.dwTrackingID) {
                isTrackedSkeletonVisible = true;
                break;
            }

            if (skeleton.Position.z < closestSkeletonDistance) {
                closestSkeleton = skeleton.dwTrackingID;
                closestSkeletonDistance = skeleton.Position.z;
            }
        }
    }

    // Now we choose a new skeleton unless the currently tracked skeleton is still visible
    if (!isTrackedSkeletonVisible && closestSkeleton != NUI_SKELETON_INVALID_TRACKING_ID) {
        HRESULT hr = m_stream->SetTrackedPlayer(closestSkeleton);
        if (FAILED(hr))
            return false;

        m_trackedSkeleton = closestSkeleton;
    }

    return true;
}
