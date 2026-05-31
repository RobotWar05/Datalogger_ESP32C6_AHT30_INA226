#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <Arduino.h>
#include "GlobalConfig.h"

class DeviceManager {
public:
    void begin();
    void update(); 
    
    void updateSensor();
    void updateDisplay();
    void updateRelay();

private:
    unsigned long lastUpdate = 0;
    bool ina226Ready = false;
    bool oledReady = false;

    void beginIna226();
    void updateIna226();
    bool readIna226Register(uint8_t reg, uint16_t &value);
};

extern DeviceManager deviceManager;

#endif
