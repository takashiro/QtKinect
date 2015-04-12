#include "kinectglobal.h"

#include "view/tnuicolorcamera.h"
#include "view/tnuitrackeritem.h"
#include "view/tnuimousearea.h"

#include "tnuisensormanager.h"

void initializeKinect(){
    qmlRegisterType<TNuiColorCamera>("Kinect", 1, 0, "TNuiColorCamera");
    qmlRegisterType<TNuiTrackerItem>("Kinect", 1, 0, "TNuiTrackerItem");
    qmlRegisterType<TNuiMouseArea>("Kinect", 1, 0, "TNuiMouseArea");

    SensorManager = TNuiSensorManager::instance();
}
