#include "tnuiinteractionstream.h"
#include "tnuidepthstream.h"
#include "tnuiskeletonstream.h"
#include "tnuisensor.h"

class TNuiFullscreenClient : public INuiInteractionClient
{
    friend class TNuiInteractionStream;

    STDMETHODIMP_(ULONG) AddRef()
    {
        ref.ref();
        return S_OK;
    }

    STDMETHODIMP_(ULONG) Release()
    {
        ref.deref();
        if (ref.load() == 0) {
            delete this;
        }
        return S_OK;
    }

    STDMETHODIMP QueryInterface(REFIID, void **)
    {
        return S_OK;
    }

    STDMETHODIMP GetInteractionInfoAtLocation(DWORD /*skeletonTrackingId*/, NUI_HAND_TYPE /*handType*/, FLOAT /*x*/, FLOAT /*y*/, NUI_INTERACTION_INFO *interactionInfo)
    {
        if (interactionInfo) {
            interactionInfo->IsGripTarget = true;
            return S_OK;
        } else {
            return E_POINTER;
        }
    }

    QAtomicInt ref;
};

TNuiInteractionStream::TNuiInteractionStream(TNuiSensor *sensor)
    : TNuiStream(sensor)
    , m_interactionClient(new TNuiFullscreenClient)
{
    m_interactionClient->AddRef();
    if (S_OK != NuiCreateInteractionStream(sensor->nativeSensor(), m_interactionClient, &m_stream))
        qFatal("Failed to create interaction stream");

    m_depthStream = new TNuiDepthStream(sensor, true);
    connect(m_depthStream, &TNuiDepthStream::readyRead, this, &TNuiInteractionStream::processDepth);

    m_skeletonStream = new TNuiSkeletonStream(sensor);
    connect(m_skeletonStream, &TNuiSkeletonStream::readyRead, this, &TNuiInteractionStream::processSkeleton);
}

TNuiInteractionStream::~TNuiInteractionStream()
{
    m_interactionClient->Release();
    m_stream->Release();

    if (m_frameReadyEvent != INVALID_HANDLE_VALUE)
        CloseHandle(m_frameReadyEvent);
}

bool TNuiInteractionStream::open()
{
    if (m_depthStream->open() && m_skeletonStream->open()) {
        m_frameReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        m_isOpen = (S_OK == m_stream->Enable(m_frameReadyEvent));
        if (m_isOpen)
            start();
    } else {
        m_isOpen = false;
    }
    qDebug(m_isOpen ? "interaction open" : "interaction not open");
    return m_isOpen;
}

bool TNuiInteractionStream::close()
{
    //@todo: close depth stream & skeleton stream here
    if (S_OK == m_stream->Disable()) {
        m_isOpen = false;
        return true;
    }
    return false;
}

void TNuiInteractionStream::readFrame(NUI_INTERACTION_FRAME &frame)
{
    m_frameMutex.lockForRead();
    frame = m_frame;
    m_frameMutex.unlock();
}

bool TNuiInteractionStream::processNewFrame()
{
    m_frameMutex.lockForWrite();
    bool result = (S_OK == m_stream->GetNextFrame(0, &m_frame));
    m_frameMutex.unlock();
    return result;
}

void TNuiInteractionStream::processDepth()
{
    NUI_IMAGE_FRAME frame;
    m_depthStream->readFrame(frame);
    m_depthStream->lockData();
    m_stream->ProcessDepth(m_depthStream->dataSize(), m_depthStream->data(), frame.liTimeStamp);
    m_depthStream->unlockData();
}

void TNuiInteractionStream::processSkeleton()
{
    NUI_SKELETON_FRAME frame;
    m_skeletonStream->readFrame(frame);
    Vector4 reading = {0};
    m_sensor->nativeSensor()->NuiAccelerometerGetCurrentReading(&reading);
    m_stream->ProcessSkeleton(NUI_SKELETON_COUNT, frame.SkeletonData, &reading, frame.liTimeStamp);
}
