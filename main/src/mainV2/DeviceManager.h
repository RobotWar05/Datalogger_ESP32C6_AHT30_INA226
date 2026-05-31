#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <Arduino.h>
#include "GlobalConfig.h"
#include "SensorManager.h"
#include "DisplayManager.h"

class DeviceManager {
public:
    void begin();
    void update(); 
    void updateRelay();

private:
    unsigned long lastUpdate = 0;
};

extern DeviceManager deviceManager;

#endif
