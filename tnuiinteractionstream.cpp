#include "tnuiinteractionstream.h"
#include "tnuidepthstream.h"
#include "tnuiskeletonstream.h"
#include "tnuisensor.h"

class TNuiFullscreenClient : public INuiInteractionClient
{
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

    STDMETHODIMP GetInteractionInfoAtLocation(DWORD skeletonTrackingId, NUI_HAND_TYPE handType, FLOAT x, FLOAT y, NUI_INTERACTION_INFO *interactionInfo)
    {
        if (skeletonTrackingId <= 0 || handType == NUI_HAND_TYPE_NONE)
            return E_FAIL;

        if (interactionInfo == NULL)
            return E_POINTER;

        Q_UNUSED(x);
        Q_UNUSED(y);

        interactionInfo->IsGripTarget = FALSE;
        interactionInfo->IsPressTarget = TRUE;
        interactionInfo->PressAttractionPointX = 0.0f;
        interactionInfo->PressAttractionPointY = 0.0f;
        interactionInfo->PressTargetControlId = 1;

        return S_OK;
    }

   QAtomicInt ref;
};

TNuiInteractionStreamInternal::TNuiInteractionStreamInternal(TNuiSensor *sensor, QObject *parent)
    : TNuiStreamInternal(sensor, parent)
    , m_interactionClient(new TNuiFullscreenClient)
{
    m_interactionClient->AddRef();
    if (S_OK != NuiCreateInteractionStream(sensor->nativeSensor(), m_interactionClient, &m_stream))
        qFatal("Failed to create interaction stream");

    m_depthStream = new TNuiDepthStream(sensor, true);
    connect(m_depthStream, &TNuiDepthStream::readyRead, this, &TNuiInteractionStreamInternal::processDepth);

    m_skeletonStream = new TNuiSkeletonStream(sensor);
    connect(m_skeletonStream, &TNuiSkeletonStream::readyRead, this, &TNuiInteractionStreamInternal::processSkeleton);
}

TNuiInteractionStreamInternal::~TNuiInteractionStreamInternal()
{
    m_interactionClient->Release();
    m_stream->Release();

    if (m_frameReadyEvent != INVALID_HANDLE_VALUE)
        CloseHandle(m_frameReadyEvent);
}

bool TNuiInteractionStreamInternal::open()
{
    if (m_depthStream->open() && m_skeletonStream->open()) {
        m_frameReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        m_isOpen = (S_OK == m_stream->Enable(m_frameReadyEvent));
        if (m_isOpen)
            start();
    } else {
        m_isOpen = false;
    }
    return m_isOpen;
}

bool TNuiInteractionStreamInternal::close()
{
    //@todo: close depth stream & skeleton stream here
    if (S_OK == m_stream->Disable()) {
        m_isOpen = false;
        return true;
    }
    return false;
}


bool TNuiInteractionStreamInternal::processNewFrame()
{
    m_frameMutex.lockForWrite();
    bool result = (S_OK == m_stream->GetNextFrame(0, &m_frame));
    m_frameMutex.unlock();
    return result;
}

void TNuiInteractionStreamInternal::processDepth()
{
    NUI_IMAGE_FRAME frame;
    m_depthStream->readFrame(frame);
    m_depthStream->lockData();
    HRESULT hr = m_stream->ProcessDepth(m_depthStream->dataSize(), m_depthStream->data(), frame.liTimeStamp);
    m_depthStream->unlockData();
    if (FAILED(hr))
        qDebug("TNuiIntreactionStream: Failed to process skeleton.");
}

void TNuiInteractionStreamInternal::processSkeleton()
{
    NUI_SKELETON_FRAME frame;
    m_skeletonStream->readFrame(frame);
    Vector4 reading = {0};
    m_sensor->nativeSensor()->NuiAccelerometerGetCurrentReading(&reading);
    HRESULT hr = m_stream->ProcessSkeleton(NUI_SKELETON_COUNT, frame.SkeletonData, &reading, frame.liTimeStamp);
    if (FAILED(hr))
        qDebug("TNuiIntreactionStream: Failed to process skeleton.");
}


QPointer<TNuiInteractionStreamInternal> TNuiInteractionStream::d = nullptr;

TNuiInteractionStream::TNuiInteractionStream(TNuiSensor *sensor)
    : TNuiStream(sensor)
{
    if (d == nullptr)
        d = new TNuiInteractionStreamInternal(sensor);
    setInternal(d);
}

TNuiInteractionStream::~TNuiInteractionStream()
{
}

void TNuiInteractionStream::readFrame(NUI_INTERACTION_FRAME &frame)
{
    d->m_frameMutex.lockForRead();
    frame = d->m_frame;
    d->m_frameMutex.unlock();
}
