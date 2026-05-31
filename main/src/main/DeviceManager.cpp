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

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED error");
    } else {
        oledReady = true;
        display.setTextColor(WHITE);
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Khoi dong...");
        display.display();
    }

    if (!aht.begin()) {
        Serial.println("AHT30 error");
        if (oledReady) {
            display.println("Loi AHT30!");
            display.display();
        }
    } else {
        Serial.println("AHT30 OK");
    }

    beginIna226();
}

void DeviceManager::update() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate >= interval) {
        lastUpdate = currentMillis;
        updateSensor();
        updateIna226();
        updateDisplay();
        updateRelay();
    }
}

void DeviceManager::updateSensor() {
    sensors_event_t humidity, temp_event;
    aht.getEvent(&humidity, &temp_event);

    if (isnan(temp_event.temperature) || isnan(humidity.relative_humidity)) {
        Serial.println("AHT30 read error");
        return;
    }

    temp = temp_event.temperature;
    hum = humidity.relative_humidity;

    Serial.print("AHT30 temp=");
    Serial.print(temp, 1);
    Serial.print(" C, hum=");
    Serial.print(hum, 1);
    Serial.println(" %");
}

void DeviceManager::updateDisplay() {
    if (!oledReady) return;

    struct tm timeinfo;
    bool timeSynced = getLocalTime(&timeinfo);

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);

    if (timeSynced) {
        display.setCursor(42, 0);
        if (timeinfo.tm_hour < 10) display.print("0");
        display.print(timeinfo.tm_hour);
        display.print(":");
        if (timeinfo.tm_min < 10) display.print("0");
        display.print(timeinfo.tm_min);
    } else {
        display.setCursor(16, 0);
        display.print("Dang cap nhat");
    }

    display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(0, 15);
    display.print((int)temp);
    display.setTextSize(1);
    display.print("o");
    display.setTextSize(2);
    display.print("C");

    display.setCursor(72, 15);
    display.print((int)hum);
    display.print("%");

    display.display();
}

void DeviceManager::updateRelay() {
    if (isAutoMode) {
        if (temp > tempHighLimit) {
            fanState = true;
        } else if (temp < tempLowLimit) {
            fanState = false;
        }
    }

    if (RELAY_FAN_PIN >= 0) digitalWrite(RELAY_FAN_PIN, fanState ? LOW : HIGH);
    if (RELAY_LIGHT_PIN >= 0) digitalWrite(RELAY_LIGHT_PIN, lightState ? LOW : HIGH);
}

void DeviceManager::beginIna226() {
    Wire.beginTransmission(INA226_ADDRESS);
    ina226Ready = (Wire.endTransmission() == 0);
    ina226Online = ina226Ready;

    if (ina226Ready) {
        Serial.println("INA226 OK on shared I2C GPIO6/GPIO7");
        if (readIna226Register(0xFE, inaManufacturerId) && readIna226Register(0xFF, inaDieId)) {
            inaErrorCode = INA226_ERROR_NONE;
            Serial.print("INA226 manufacturer=0x");
            Serial.print(inaManufacturerId, HEX);
            Serial.print(", die=0x");
            Serial.println(inaDieId, HEX);
        } else {
            inaErrorCode = INA226_ERROR_ID_READ;
            Serial.println("INA226 ID read error");
        }
    } else {
        inaErrorCode = INA226_ERROR_NOT_FOUND;
        Serial.println("INA226 not found on shared I2C GPIO6/GPIO7");
    }
}

void DeviceManager::updateIna226() {
    if (!ina226Ready) {
        Wire.beginTransmission(INA226_ADDRESS);
        ina226Ready = (Wire.endTransmission() == 0);
        if (!ina226Ready) {
            ina226Online = false;
            inaErrorCode = INA226_ERROR_NOT_FOUND;
            return;
        }
    }

    uint16_t busRaw = 0;
    uint16_t shuntRawUnsigned = 0;
    if (!readIna226Register(0x02, busRaw)) {
        Serial.println("INA226 bus voltage register read error");
        ina226Ready = false;
        ina226Online = false;
        inaErrorCode = INA226_ERROR_BUS_READ;
        return;
    }

    if (!readIna226Register(0x01, shuntRawUnsigned)) {
        Serial.println("INA226 shunt voltage register read error");
        ina226Ready = false;
        ina226Online = false;
        inaErrorCode = INA226_ERROR_SHUNT_READ;
        return;
    }

    int16_t shuntRaw = (int16_t)shuntRawUnsigned;
    float busVoltage = busRaw * 0.00125f;
    float shuntVoltage = shuntRaw * 0.0000025f;
    float current = shuntVoltage / INA226_SHUNT_OHMS;
    inaBusRaw = busRaw;
    inaShuntRaw = shuntRaw;
    inaBusVoltage = busVoltage;
    inaCurrent = current;
    ina226Online = true;
    inaErrorCode = INA226_ERROR_NONE;

    Serial.print("INA226 bus=");
    Serial.print(busVoltage, 3);
    Serial.print(" V raw=");
    Serial.print(busRaw);
    Serial.print(" V, shunt=");
    Serial.print(shuntVoltage * 1000.0f, 3);
    Serial.print(" mV raw=");
    Serial.print(shuntRaw);
    Serial.print(" mV, current~=");
    Serial.print(current, 3);
    Serial.println(" A");
}

bool DeviceManager::readIna226Register(uint8_t reg, uint16_t &value) {
    Wire.beginTransmission(INA226_ADDRESS);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) return false;

    if (Wire.requestFrom(INA226_ADDRESS, (uint8_t)2) != 2) return false;

    uint16_t high = Wire.read();
    uint16_t low = Wire.read();
    value = (uint16_t)((high << 8) | low);
    return true;
}
