#include "tnuiskeletonstream.h"
#include "tnuisensor.h"

#include <QAtomicInt>

TNuiSkeletonStreamInternal::TNuiSkeletonStreamInternal(TNuiSensor *sensor)
   : TNuiStreamInternal(sensor)
{
    m_frameReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

TNuiSkeletonStreamInternal::~TNuiSkeletonStreamInternal()
{
    stop();
}

bool TNuiSkeletonStreamInternal::open()
{
   if (isRunning())
       return true;

   m_isOpen = S_OK == m_sensor->nativeSensor()->NuiSkeletonTrackingEnable(m_frameReadyEvent, flags);
   if (m_isOpen)
       start();
   return m_isOpen;
}

bool TNuiSkeletonStreamInternal::close()
{
    return S_OK == m_sensor->nativeSensor()->NuiSkeletonTrackingDisable();
}

bool TNuiSkeletonStreamInternal::processNewFrame()
{
    // Attempt to get the color frame
    frameMutex.lockForWrite();
    bool isValid = (S_OK == m_sensor->nativeSensor()->NuiSkeletonGetNextFrame(0, &frame));
    frameMutex.unlock();
    return isValid;
}

QPointer<TNuiSkeletonStreamInternal> TNuiSkeletonStream::d = nullptr;

TNuiSkeletonStream::TNuiSkeletonStream(TNuiSensor *sensor, TrackingFlags flags)
    : TNuiStream(sensor)
{
    if (d == nullptr)
        d = new TNuiSkeletonStreamInternal(sensor);
    setInternal(d);
    d->flags = flags;
    connect(d.data(), &TNuiSkeletonStreamInternal::readyRead, this, &TNuiSkeletonStream::readyRead);
}

TNuiSkeletonStream::~TNuiSkeletonStream()
{
}

void TNuiSkeletonStream::setFlags(TrackingFlags flags)
{
    d->flags |= flags;
}

void TNuiSkeletonStream::resetFlags(TrackingFlags flags)
{
    d->flags &= ~flags;
}

TNuiSkeletonStream::TrackingFlags TNuiSkeletonStream::flags() const
{
    return d->flags;
}

void TNuiSkeletonStream::readFrame(NUI_SKELETON_FRAME &frame)
{
    d->frameMutex.lockForRead();
    frame = d->frame;
    d->frameMutex.unlock();
}
