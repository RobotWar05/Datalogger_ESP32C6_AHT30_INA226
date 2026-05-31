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

private:
    void loadSettings();

    void setupConfigMode();
    void setupNormalMode();

    void registerConfigHandlers();
    void registerNormalHandlers();
};

extern WifiApp wifiApp;

#endif
