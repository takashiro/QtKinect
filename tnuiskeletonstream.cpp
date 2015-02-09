#include "tnuiskeletonstream.h"
#include "tnuisensor.h"

TNuiSkeletonStream::TNuiSkeletonStream(TNuiSensor *sensor, TrackingFlags flags)
    : TNuiStream(sensor)
    , m_flags(flags)
{
}

bool TNuiSkeletonStream::open()
{
    return m_sensor->openSkeletionStream(this, m_flags);
}

bool TNuiSkeletonStream::close()
{
    return m_sensor->closeSkeletonStream();
}

void TNuiSkeletonStream::readFrame(NUI_SKELETON_FRAME &frame)
{
    m_frameMutex.lock();
    frame = m_frame;
    m_frameMutex.unlock();
}

bool TNuiSkeletonStream::processNewFrame()
{
    // Attempt to get the color frame
    m_frameMutex.lock();
    bool isValid = m_sensor->readSkeletonFrame(0, m_frame);
    m_frameMutex.unlock();
    return isValid;
}