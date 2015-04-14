#include "tnuiskeletonstream.h"
#include "tnuisensor.h"

#include <QAtomicInt>

TNuiSkeletonStreamPrivate::TNuiSkeletonStreamPrivate(TNuiSensor *sensor)
   : TNuiStream(sensor)
{
    m_frameReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

TNuiSkeletonStreamPrivate::~TNuiSkeletonStreamPrivate()
{
    stop();
}

bool TNuiSkeletonStreamPrivate::open()
{
   if (isRunning())
       return true;

   bool open = (S_OK == m_sensor->nativeSensor()->NuiSkeletonTrackingEnable(m_frameReadyEvent, flags));
   if (open)
       start();
   return open;
}

bool TNuiSkeletonStreamPrivate::processNewFrame()
{
   // Attempt to get the color frame
   frameMutex.lockForWrite();
   bool isValid = (S_OK == m_sensor->nativeSensor()->NuiSkeletonGetNextFrame(0, &frame));
   frameMutex.unlock();
   return isValid;
}

TNuiSkeletonStreamPrivate *TNuiSkeletonStream::p_ptr = nullptr;

TNuiSkeletonStream::TNuiSkeletonStream(TNuiSensor *sensor, TrackingFlags flags)
    : QObject(sensor)
{
    if (p_ptr == nullptr) {
        p_ptr = new TNuiSkeletonStreamPrivate(sensor);
        //Reference count applied. The object shouldn't be deleted otherwise.
        p_ptr->setParent(nullptr);
    }

    p_ptr->ref.ref();
    p_ptr->flags = flags;
    connect(p_ptr, &TNuiSkeletonStreamPrivate::readyRead, this, &TNuiSkeletonStream::readyRead);
}

TNuiSkeletonStream::~TNuiSkeletonStream()
{
    p_ptr->ref.deref();
    if (p_ptr->ref.load() == 0) {
        delete p_ptr;
        p_ptr = nullptr;
    }
}

bool TNuiSkeletonStream::open()
{
    return p_ptr->open();
}

void TNuiSkeletonStream::tryOpen()
{
    p_ptr->tryOpen();
}

bool TNuiSkeletonStream::close()
{
    return S_OK == p_ptr->m_sensor->nativeSensor()->NuiSkeletonTrackingDisable();
}

bool TNuiSkeletonStream::reopen()
{
    return close() && open();
}

void TNuiSkeletonStream::setFlags(TrackingFlags flags)
{
    p_ptr->flags |= flags;
}

void TNuiSkeletonStream::resetFlags(TrackingFlags flags)
{
    p_ptr->flags &= ~flags;
}

TNuiSkeletonStream::TrackingFlags TNuiSkeletonStream::flags() const
{
    return p_ptr->flags;
}

void TNuiSkeletonStream::readFrame(NUI_SKELETON_FRAME &frame)
{
    p_ptr->frameMutex.lockForRead();
    frame = p_ptr->frame;
    p_ptr->frameMutex.unlock();
}
