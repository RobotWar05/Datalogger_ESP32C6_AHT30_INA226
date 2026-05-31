#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include "GlobalConfig.h"

class DisplayManager {
public:
    void begin();
    bool isReady() const;
    void showBoot();
    void showAhtError();
    void showWifiConnecting(const String &ssid, int remainingSeconds);
    void showWifiConnected(const IPAddress &ip);
    void showWifiTimeout();
    void showConfigMode();
    void showFactoryResetCountdown(int remainingSeconds);
    void showHardwareResetCountdown(unsigned long remainingSeconds);
    void showResetting();
    void updateRunScreen();

private:
    bool oledReady = false;
    void drawCenteredText(const String &text, int16_t y, uint8_t textSize);
    void drawCenteredValueLine(const String &text, int16_t y);
};

extern DisplayManager displayManager;

#endif
