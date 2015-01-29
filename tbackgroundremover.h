#ifndef BACKGROUNDREMOVER_H
#define BACKGROUNDREMOVER_H


#include "tnuisensorselector.h"

#include <QObject>

#include <Windows.h>
#include <NuiApi.h>
#include <KinectBackgroundRemoval.h>
#include <d2d1.h>

struct INuiSensor;

class TBackgroundRemover : public QObject
{
    Q_OBJECT

    static const int BYTES_PER_PIXEL    = 4;

    static const NUI_IMAGE_RESOLUTION DEPTH_RESOLUTION = NUI_IMAGE_RESOLUTION_320x240;

    // green screen background will also be scaled to this resolution
    static const NUI_IMAGE_RESOLUTION COLOR_RESOLUTION = NUI_IMAGE_RESOLUTION_640x480;

    static const int STATUS_MESSAGE_MAX_LENGTH = MAX_PATH*2;

public:
    TBackgroundRemover();
    ~TBackgroundRemover();

    /// <summary>
    /// Handles window messages, passes most to the class instance to handle
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Handle windows messages for a class instance
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    LRESULT CALLBACK        DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Creates the main window and begins processing
    /// </summary>
    /// <param name="hInstance"></param>
    /// <param name="nCmdShow"></param>
    int                     Run(HINSTANCE hInstance, int nCmdShow);

    /// <summary>
    /// Called on Kinect device status changed. It will update the sensor chooser UI control
    /// based on the new sensor status. It may also updates the sensor instance if needed
    /// </summary>
    static void CALLBACK StatusChangeCallback(HRESULT, const OLECHAR*, const OLECHAR*, void* userData);

private:
    HWND m_hWnd;
    bool m_nearMode;

    // Current Kinect
    INuiSensor *m_nuiSensor;

    // Direct2D
    ID2D1Factory* m_pD2DFactory;

    HANDLE m_depthStreamHandle;
    HANDLE m_nextDepthFrameEvent;
    HANDLE m_colorStreamHandle;
    HANDLE m_nextColorFrameEvent;
    HANDLE m_nextSkeletonFrameEvent;
    HANDLE m_nextBackgroundRemovedFrameEvent;

    uchar* m_backgroundRGBX;
    uchar* m_outputRGBX;

    INuiBackgroundRemovedColorStream *m_backgroundRemovalStream;

    TNuiSensorSelector *m_sensorSelector;

    uint m_colorWidth;
    uint m_colorHeight;
    uint m_depthWidth;
    uint m_depthHeight;
    ulong m_trackedSkeleton;

    // Create the first connected Kinect found
    bool createFirstConnected();

    // Create the stream that does removes the background and display a single player specified
    bool createBackgroundRemovedColorStream();

    /// <summary>
    /// Handle new depth data
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                 processDepth();

    /// <summary>
    /// Handle new color data
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    bool processColor();

    /// <summary>
    /// Handle new skeleton frame data
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    bool processSkeleton();

    /// <summary>
    /// compose the background removed color image with the background image
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    bool composeImage();

    /// <summary>
    /// Use the sticky player logic to determine the player whom the background removed
    /// color stream should consider as foreground.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                 chooseSkeleton(NUI_SKELETON_DATA* pSkeletonData);

    /// <summary>
    /// Update the sensor and status based on the input changed flags
    /// </summary>
    /// <param name="changedFlags">The device change flags</param>
    void updateSensorAndStatus(TNuiSensorSelector::ChangeFlag changedFlags);

signals:
    void toastMessage(const QString &message);
    void newFrame(const uchar *frame, uint length);
    void statusChanged(ulong status);
};

#endif // BACKGROUNDREMOVER_H
