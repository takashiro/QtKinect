#ifndef TNUISENSOR_H
#define TNUISENSOR_H

#include <QObject>

#include <Windows.h>
#include <NuiApi.h>

#include "tnuiimagestream.h"
#include "tnuiskeletonstream.h"

class TNuiSensor : public QObject
{
    Q_OBJECT

public:
    friend class TNuiSensorManager;

    enum State{
        ConnectedState = S_OK,
        //The device is connected, but still initializing.
        InitializingState = S_NUI_INITIALIZING,
        //The device is not connected.
        NotConnectedState = E_NUI_NOTCONNECTED,
        //The device is not a valid Kinect.
        NotGenuine = E_NUI_NOTGENUINE,
        //The device is an unsupported model.
        NotSupportedState = E_NUI_NOTSUPPORTED,
        //The device is connected to a hub without the necessary bandwidth requirements.
        InsufficientBandwidthState = E_NUI_INSUFFICIENTBANDWIDTH,
        //The device is connected, but unpowered.
        NotPoweredState = E_NUI_NOTPOWERED,
        //There was some other unspecified error.
        NotReadyState = E_NUI_NOTREADY
    };

    enum InitializeFlag{
        //This flag was deprecated in version 1.5; it is no longer used.
        DefaultHardWareThreadFlag = NUI_INITIALIZE_DEFAULT_HARDWARE_THREAD,
        //Initialize the sensor to provide audio data.
        UseAudioInitFlag = NUI_INITIALIZE_FLAG_USES_AUDIO,
        //Initialize the sensor to provide color data.
        UseColorFlag = NUI_INITIALIZE_FLAG_USES_COLOR,
        //Initialize the sensor to provide depth data.
        UseDepthFlag = NUI_INITIALIZE_FLAG_USES_DEPTH,
        //Initialize the sensor to provide depth data with a player index.
        UseDepthAndPlayerIndexFlag = NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX,
        //Initialize the sensor to provide skeleton data.
        UseSkeletonFlag = NUI_INITIALIZE_FLAG_USES_SKELETON
    };

    ~TNuiSensor();

    INuiSensor *nativeSensor() const { return m_sensor; }

    //Initializes the Kinect.
    void initialize(uint flags);

    //Turns the sensor off. No action is taken if the sensor is already off.
    void shutdown() {m_sensor->NuiShutdown();}

    //Gets the flags that were passed to the TNuiSensor::initialize method.
    uint initFlags() const {return m_sensor->NuiInitializationFlags();}

    //Gets the connection status of the Kinect sensor.
    State state() {return m_state;}

    //Gets the zero-based sensor index.
    int instanceIndex() const {return m_sensor->NuiInstanceIndex();}

    //Gets the Kinect sensor connection ID.
    QString deviceConnectionId() const {return m_deviceConnectionId;}

    /*
        NuiAccelerometerGetCurrentReading	Gets the accelerometer reading.
        NuiAudioArrayId	Gets the USB device name of the audio array.
        NuiCameraElevationGetAngle	Gets the elevation (tilt angle) whose range is [+/-27] degrees.
        NuiCameraElevationSetAngle	Sets the elevation (tilt angle) whose range is [+/-27] degrees.
        NuiGetAudioSource	Gets the audio source.
        NuiGetColorCameraSettings	Gets the color camera settings.
        NuiGetCoordinateMapper	Gets the coordinate mapping settings.
        NuiGetForceInfraredEmitterOff	Gets a boolean value that indicates whether the infrared emitter is disabled. The default is FALSE, that is, the emitter is NOT disabled.
        NuiImageFrameGetDepthImagePixelFrameTexture	Returns a pointer to an INuiFrameTexture that contains the data of a depth frame in NUI_DEPTH_IMAGE_PIXEL format (instead of packed USHORTs).
        NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution	Gets the pixel coordinates in color space that correspond to the specified pixel coordinates in depth space, using the specified depth resolution.
        NuiImageGetColorPixelCoordinatesFromDepthPixel	Gets the pixel coordinates in color space that correspond to the specified pixel coordinates in depth space.
        NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution	Gets the pixel coordinates in color space that correspond to the specified pixel coordinates in depth space, using the specified depth resolution.
        NuiImageStreamGetImageFrameFlags	Gets the image frame options.
        NuiImageStreamSetImageFrameFlags	Sets the image frame options.
        NuiSetForceInfraredEmitterOff	Sets a value indicating whether the infrared emitter is disabled. The default value is false, which means that the infrared sensor is enabled (that is, not disabled).
        NuiSetFrameEndEvent	Sets the event that signals the last frame.
        NuiTransformSmooth	Filters skeleton positions to reduce jitter between frames.
        NuiUniqueId	Do not use this method.
        QueryInterface	Gets the supported interfaces.
    */

signals:
    void stateChanged();
    void connected();
    void disconnected();

protected:
    TNuiSensor(INuiSensor *sensor, QObject *parent = 0);
    void _updateState();

    State m_state;

private:
    INuiSensor *m_sensor;
    QString m_deviceConnectionId;
};

#endif // TNUISENSOR_H
