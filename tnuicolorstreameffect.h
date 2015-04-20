#ifndef TNUICOLORSTREAMEFFECT_H
#define TNUICOLORSTREAMEFFECT_H

#include <QObject>

class TNuiColorStream;

class TNuiColorStreamEffect : public QObject
{
    Q_OBJECT

public:
    TNuiColorStreamEffect(TNuiColorStream *stream);

    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    void draw(uchar *data, uint length);

signals:
    void enabledChanged(bool enabled);

protected:
    virtual void updateFrameData(uchar *data, uint length) = 0;

    TNuiColorStream *m_colorStream;
    bool m_enabled;
};

#endif // TNUICOLORSTREAMEFFECT_H
