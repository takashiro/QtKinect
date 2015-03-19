#include "tnuiskeletonstream.h"
#include "tnuisensor.h"

TNuiSkeletonStream::TNuiSkeletonStream(TNuiSensor *sensor, TrackingFlags flags)
    : TNuiStream(sensor)
    , m_flags(flags)
{
}

TNuiSkeletonStream::~TNuiSkeletonStream()
{
    m_sensor->_closeSkeletonStream();
}

bool TNuiSkeletonStream::open()
{
    bool open = m_sensor->_openSkeletionStream(this, m_flags);
    if (open)
        start();
    return open;
}

bool TNuiSkeletonStream::close()
{
    return m_sensor->_closeSkeletonStream();
}

bool TNuiSkeletonStream::reopen()
{
    return m_sensor->_closeSkeletonStream() && m_sensor->_openSkeletionStream(this, m_flags);
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
    bool isValid = m_sensor->_readSkeletonFrame(0, m_frame);
    m_frameMutex.unlock();
    return isValid;
}
