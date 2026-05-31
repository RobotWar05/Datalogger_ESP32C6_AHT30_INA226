#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "GlobalConfig.h"

class SensorManager {
public:
    void begin();
    void update();
    bool hasAhtData() const;
    bool hasInaData() const;

private:
    bool ahtReady = false;
    bool ina226Ready = false;

    void beginAht30();
    void beginIna226();
    void updateAht30();
    void updateIna226();
    void markInaOffline(uint8_t errorCode);
    bool readIna226Register(uint8_t reg, uint16_t &value);
};

extern SensorManager sensorManager;

#endif
