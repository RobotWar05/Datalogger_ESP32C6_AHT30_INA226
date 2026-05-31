#ifndef GOOGLE_LOGGER_H
#define GOOGLE_LOGGER_H

#include <Arduino.h>
#include "GlobalConfig.h"

class GoogleLogger {
public:
    void update();
    void setIntervalMs(unsigned long value);
    unsigned long getIntervalMs() const;

private:
    unsigned long lastLogTime = 0;
    unsigned long logIntervalMs = 300000UL;

    String formatQueryFloat(float value, uint8_t digits) const;
};

extern GoogleLogger googleLogger;

#endif
