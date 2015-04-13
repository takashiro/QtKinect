#ifndef TNUIBACKGROUNDREMOVEDCOLORSTREAMPRIVATE_H
#define TNUIBACKGROUNDREMOVEDCOLORSTREAMPRIVATE_H

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

#endif // TNUIBACKGROUNDREMOVEDCOLORSTREAMPRIVATE_H
