#include "SensorManager.h"

SensorManager sensorManager;

void SensorManager::begin() {
    beginAht30();
    beginIna226();
}

void SensorManager::update() {
    updateAht30();
    updateIna226();
}

bool SensorManager::hasAhtData() const {
    return ahtReady && isfinite(temp) && isfinite(hum);
}

bool SensorManager::hasInaData() const {
    return ina226Online && isfinite(inaBusVoltage) && isfinite(inaCurrent);
}

void SensorManager::beginAht30() {
    ahtReady = aht.begin();
    if (!ahtReady) {
        temp = NAN;
        hum = NAN;
        Serial.println("AHT30 error");
        return;
    }

    Serial.println("AHT30 OK");
}

void SensorManager::beginIna226() {
    Wire.beginTransmission(INA226_ADDRESS);
    ina226Ready = (Wire.endTransmission() == 0);
    ina226Online = ina226Ready;

    if (!ina226Ready) {
        markInaOffline(INA226_ERROR_NOT_FOUND);
        Serial.println("INA226 not found on shared I2C GPIO6/GPIO7");
        return;
    }

    Serial.println("INA226 OK on shared I2C GPIO6/GPIO7");
    if (readIna226Register(0xFE, inaManufacturerId) && readIna226Register(0xFF, inaDieId)) {
        inaErrorCode = INA226_ERROR_NONE;
        Serial.print("INA226 manufacturer=0x");
        Serial.print(inaManufacturerId, HEX);
        Serial.print(", die=0x");
        Serial.println(inaDieId, HEX);
    } else {
        markInaOffline(INA226_ERROR_ID_READ);
        Serial.println("INA226 ID read error");
    }
}

void SensorManager::updateAht30() {
    if (!ahtReady) {
        beginAht30();
        return;
    }

    sensors_event_t humidity, tempEvent;
    aht.getEvent(&humidity, &tempEvent);

    if (!isfinite(tempEvent.temperature) || !isfinite(humidity.relative_humidity)) {
        Serial.println("AHT30 read error");
        temp = NAN;
        hum = NAN;
        return;
    }

    temp = tempEvent.temperature;
    hum = humidity.relative_humidity;

    Serial.print("AHT30 temp=");
    Serial.print(temp, 1);
    Serial.print(" C, hum=");
    Serial.print(hum, 1);
    Serial.println(" %");
}

void SensorManager::updateIna226() {
    if (!ina226Ready) {
        Wire.beginTransmission(INA226_ADDRESS);
        ina226Ready = (Wire.endTransmission() == 0);
        if (!ina226Ready) {
            markInaOffline(INA226_ERROR_NOT_FOUND);
            return;
        }
    }

    uint16_t busRaw = 0;
    uint16_t shuntRawUnsigned = 0;
    if (!readIna226Register(0x02, busRaw)) {
        Serial.println("INA226 bus voltage register read error");
        ina226Ready = false;
        markInaOffline(INA226_ERROR_BUS_READ);
        return;
    }

    if (!readIna226Register(0x01, shuntRawUnsigned)) {
        Serial.println("INA226 shunt voltage register read error");
        ina226Ready = false;
        markInaOffline(INA226_ERROR_SHUNT_READ);
        return;
    }

    const int16_t shuntRaw = (int16_t)shuntRawUnsigned;
    const float busVoltage = busRaw * 0.00125f;
    const float shuntVoltage = shuntRaw * 0.0000025f;
    const float current = shuntVoltage / INA226_SHUNT_OHMS;

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
    Serial.print(", shunt=");
    Serial.print(shuntVoltage * 1000.0f, 3);
    Serial.print(" mV raw=");
    Serial.print(shuntRaw);
    Serial.print(", current~=");
    Serial.print(current, 3);
    Serial.println(" A");
}

void SensorManager::markInaOffline(uint8_t errorCode) {
    ina226Online = false;
    inaErrorCode = errorCode;
    inaBusVoltage = NAN;
    inaCurrent = NAN;
}

bool SensorManager::readIna226Register(uint8_t reg, uint16_t &value) {
    Wire.beginTransmission(INA226_ADDRESS);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) return false;

    if (Wire.requestFrom(INA226_ADDRESS, (uint8_t)2) != 2) return false;

    uint16_t high = Wire.read();
    uint16_t low = Wire.read();
    value = (uint16_t)((high << 8) | low);
    return true;
}
