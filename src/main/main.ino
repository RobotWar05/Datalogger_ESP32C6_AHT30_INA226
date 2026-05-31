#include <Arduino.h>
#include "GlobalConfig.h"
#include "DeviceManager.h"
#include "WifiApp.h"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 0;
const long interval = 2000;

WebServer server(80);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_AHTX0 aht;
Preferences preferences;

float temp = 0;
float hum = 0;
float inaBusVoltage = 0;
float inaCurrent = 0;
bool ina226Online = false;
uint16_t inaBusRaw = 0;
int16_t inaShuntRaw = 0;
uint16_t inaManufacturerId = 0;
uint16_t inaDieId = 0;
uint8_t inaErrorCode = INA226_ERROR_NOT_FOUND;
bool isConfigMode = false;
unsigned long pressTime = 0;
bool isPressing = false;

bool isAutoMode = true;   
bool fanState = false;    
bool lightState = false; 

float tempHighLimit = 32.0;
float tempLowLimit = 18.0;

void setup() {
  Serial.begin(115200);
  
  esp_task_wdt_config_t twdt_config = {
      .timeout_ms = 5000,
      .idle_core_mask = (1 << 0), 
      .trigger_panic = true,
  };
  esp_task_wdt_init(&twdt_config);
  esp_task_wdt_add(NULL);
  
  deviceManager.begin();
  wifiApp.begin();
}

void loop() {
  esp_task_wdt_reset();

  if (wifiApp.resetCheck()) {
    return;
  }

  wifiApp.handle();
  
  if (!isConfigMode) {
      wifiApp.sendToGoogleSheet();
  }

  if (!isConfigMode) {
     deviceManager.update();
  }
}
