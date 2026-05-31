#ifndef WIFI_APP_H
#define WIFI_APP_H

#include <Arduino.h>
#include "GlobalConfig.h"
#include "HtmlPages.h"

class WifiApp {
public:
    void begin();
    void handle();
    bool resetCheck();
    
    void sendToGoogleSheet();

private:
    unsigned long lastLogTime = 0;
    void writeString(int add, String data);
    String readString(int add);
    void clearEEPROM();
    void loadSettings();
    void saveSettingsToEEPROM(float h, float l);

    void setupConfigMode();
    void setupNormalMode();

    void registerConfigHandlers();
    void registerNormalHandlers();
};

extern WifiApp wifiApp;

#endif
