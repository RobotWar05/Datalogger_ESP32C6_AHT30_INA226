#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>
#include <Preferences.h>
#include <esp_task_wdt.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>


#define I2C_SDA 6
#define I2C_SCL 7

#define INA226_SDA I2C_SDA
#define INA226_SCL I2C_SCL
#define RESET_PIN 0
#define RESET_HOLD_MS 5000UL
#define WIFI_CONNECT_TIMEOUT_MS 10000UL

#define LED_PIN -1

#define RELAY_FAN_PIN -1
#define RELAY_LIGHT_PIN -1

#define BUTTON_1_PIN 2
#define BUTTON_2_PIN 3
#define BUTTON_3_PIN 1
#define BUTTON_4_PIN 14
#define BUTTON_5_PIN 0
#define BUTTON_6_PIN 18

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define INA226_ADDRESS 0x40
#define INA226_SHUNT_OHMS 0.1f

#define INA226_ERROR_NONE 0
#define INA226_ERROR_NOT_FOUND 1
#define INA226_ERROR_BUS_READ 2
#define INA226_ERROR_SHUNT_READ 3
#define INA226_ERROR_ID_READ 4

extern const char *ntpServer;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;
extern const long interval;

extern WebServer server;
extern Adafruit_SSD1306 display;
extern Adafruit_AHTX0 aht;
extern Preferences preferences;

extern float temp;
extern float hum;
extern float inaBusVoltage;
extern float inaCurrent;
extern bool ina226Online;
extern uint16_t inaBusRaw;
extern int16_t inaShuntRaw;
extern uint16_t inaManufacturerId;
extern uint16_t inaDieId;
extern uint8_t inaErrorCode;
extern bool isConfigMode;
extern unsigned long pressTime;
extern bool isPressing;

extern bool isAutoMode;   
extern bool fanState;    
extern bool lightState; 

extern float tempHighLimit;
extern float tempLowLimit;

#endif
