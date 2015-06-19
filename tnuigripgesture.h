#ifndef TNUIGRIPGESTURE_H
#define TNUIGRIPGESTURE_H

#include "tnuigesture.h"

class TNuiGripGesture : public TNuiGesture
{
public:
    enum Type{
        LeftHand,
        RightHand
    };

    TNuiGripGesture(Type type, QObject *parent = 0);
    ~TNuiGripGesture();

    Type type() const { return m_type; }

protected:
    Type m_type;
};

#endif // TNUIGRIPGESTURE_H
