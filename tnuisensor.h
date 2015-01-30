#ifndef TNUISENSOR_H
#define TNUISENSOR_H

#include <Windows.h>
#include <NuiApi.h>

class TNuiSensor : public INuiSensor
{
public:
    enum Status{

    };

    TNuiSensor();

signals:
    statusChanged(Status status);
};

#endif // TNUISENSOR_H
