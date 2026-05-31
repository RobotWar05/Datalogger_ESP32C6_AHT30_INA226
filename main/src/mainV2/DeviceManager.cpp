#include "DeviceManager.h"

DeviceManager deviceManager;

void DeviceManager::begin() {
    pinMode(RESET_PIN, INPUT_PULLUP);
    if (LED_PIN >= 0) pinMode(LED_PIN, OUTPUT);
    if (RELAY_FAN_PIN >= 0) pinMode(RELAY_FAN_PIN, OUTPUT);
    if (RELAY_LIGHT_PIN >= 0) pinMode(RELAY_LIGHT_PIN, OUTPUT);

    if (RELAY_FAN_PIN >= 0) digitalWrite(RELAY_FAN_PIN, HIGH);
    if (RELAY_LIGHT_PIN >= 0) digitalWrite(RELAY_LIGHT_PIN, HIGH);

    fanState = false;
    lightState = false;
    isAutoMode = true;

    Wire.begin(I2C_SDA, I2C_SCL);
    displayManager.begin();
    sensorManager.begin();
}

void DeviceManager::update() {
    const unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate < interval) return;

    lastUpdate = currentMillis;
    sensorManager.update();
    updateRelay();
    displayManager.updateRunScreen();
}

void DeviceManager::updateRelay() {
    if (isAutoMode) {
        if (isfinite(temp) && temp > tempHighLimit) {
            fanState = true;
        } else if (isfinite(temp) && temp < tempLowLimit) {
            fanState = false;
        }
    }

    if (RELAY_FAN_PIN >= 0) digitalWrite(RELAY_FAN_PIN, fanState ? LOW : HIGH);
    if (RELAY_LIGHT_PIN >= 0) digitalWrite(RELAY_LIGHT_PIN, lightState ? LOW : HIGH);
}
