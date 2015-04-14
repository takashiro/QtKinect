#ifndef TNUIBACKGROUNDREMOVEDCOLORSTREAMPRIVATE_H
#define TNUIBACKGROUNDREMOVEDCOLORSTREAMPRIVATE_H

#ifdef KINECT_USE_BACKGROUNDREMOVAL

#include "tnuistream.h"

struct INuiBackgroundRemovedColorStream;
class TNuiBackgroundRemovedColorStream;

class TNuiBackgroundRemovedColorStreamPrivate : public TNuiStream
{
    Q_OBJECT

    friend class TNuiBackgroundRemovedColorStream;

    TNuiBackgroundRemovedColorStreamPrivate(TNuiBackgroundRemovedColorStream *stream);
    ~TNuiBackgroundRemovedColorStreamPrivate();

    bool open();
    bool processNewFrame();

    TNuiBackgroundRemovedColorStream *stream;
    INuiBackgroundRemovedColorStream *nativeStream;
};

#endif

#endif // TNUIBACKGROUNDREMOVEDCOLORSTREAMPRIVATE_H
