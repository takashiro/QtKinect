#ifndef TNUIGESTURE_H
#define TNUIGESTURE_H

#include <QObject>

class TNuiSkeletonStream;
class TNuiDepthStream;

class TNuiGesture : public QObject
{
public:
    enum State{
        Unknown = -1,
        Off = 0,
        On = 1
    };

    TNuiGesture(QObject *parent = 0);
    ~TNuiGesture();

    State state() const { return m_state; }

signals:
    void on();
    void off();
    void stateChanged();

protected:
    State m_state;

    static TNuiSkeletonStream *m_skeletonStream;
    static TNuiDepthStream *m_depthStream;

private:
    void processDepth();
    void processSkeleton();
};

#endif // TNUIGESTURE_H
