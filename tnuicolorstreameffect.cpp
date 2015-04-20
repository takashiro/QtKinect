#include "tnuicolorstreameffect.h"
#include "tnuicolorstream.h"

TNuiColorStreamEffect::TNuiColorStreamEffect(TNuiColorStream *stream)
    : QObject(stream)
    , m_colorStream(stream)
    , m_enabled(true)
{
}

void TNuiColorStreamEffect::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged(enabled);
    }
}

void TNuiColorStreamEffect::draw(uchar *data, uint length)
{
    if (m_enabled)
        updateFrameData(data, length);
}
