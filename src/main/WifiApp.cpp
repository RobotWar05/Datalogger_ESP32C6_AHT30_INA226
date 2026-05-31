#include "WifiApp.h"
#include "WifiConfigPage.h"

WifiApp wifiApp;

unsigned long resetPressStartTime = 0;
bool isResetPressing = false;
int lastResetCountdown = -1;
unsigned long logInterval = 300000; 

static bool isResetButtonPressed() {
    return digitalRead(RESET_PIN) == LOW;
}

void WifiApp::begin() {
    preferences.begin("my-app", false);
    
    String saved_ssid = preferences.getString("ssid", "");
    String saved_pass = preferences.getString("pass", "");

    if (saved_ssid.length() > 1) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(saved_ssid.c_str(), saved_pass.c_str());

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Ket noi Wifi...");
        display.println(saved_ssid);
        display.display();

        int retries = 0;
        int lastRemaining = -1;
        const int maxRetries = WIFI_CONNECT_TIMEOUT_MS / 500;
        while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
            esp_task_wdt_reset(); 
            int remaining = (maxRetries - retries + 1) / 2;
            if (remaining != lastRemaining) {
                lastRemaining = remaining;
                Serial.print("Connecting WiFi, timeout in ");
                Serial.print(remaining);
                Serial.println("s");

                display.clearDisplay();
                display.setTextColor(WHITE);
                display.setTextSize(1);
                display.setCursor(0, 0);
                display.println("Ket noi Wifi...");
                display.println(saved_ssid);
                display.print("Timeout: ");
                display.print(remaining);
                display.println("s");
                display.display();
            }

            delay(500);
            Serial.print(".");
            retries++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            WiFi.setSleep(false); 
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Wifi OK!");
            display.println(WiFi.localIP());
            display.display();
            delay(2000);
            setupNormalMode();
        } else {
            Serial.println("WiFi connect timeout. Clearing saved WiFi and restarting...");
            display.clearDisplay();
            display.setTextColor(WHITE);
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.println("Wifi timeout!");
            display.println("Xoa cau hinh cu");
            display.println("Khoi dong lai...");
            display.display();

            preferences.remove("ssid");
            preferences.remove("pass");
            delay(1200);
            ESP.restart();
        }
    } else {
        setupConfigMode();
    }
}

void WifiApp::handle() {
    server.handleClient();
}

void WifiApp::loadSettings() {
    tempHighLimit = preferences.getFloat("tempHigh", 32.0);
    tempLowLimit = preferences.getFloat("tempLow", 18.0);
    
    if (tempHighLimit < -50 || tempHighLimit > 100) tempHighLimit = 32.0;
    if (tempLowLimit < -50 || tempLowLimit > 100) tempLowLimit = 18.0;

    long savedInterval = preferences.getLong("interval", 5000);
    if(savedInterval < 2000) savedInterval = 5000;
    logInterval = savedInterval;
}

void WifiApp::setupConfigMode() {
    isConfigMode = true;
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("HE THONG GIAM SAT");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("CHE DO CAU HINH");
    display.println("----------------");
    display.println("Wifi: HE THONG...");
    display.println("IP: 192.168.4.1");
    display.display();
    registerConfigHandlers();
    server.begin();
}

void WifiApp::setupNormalMode() {
    isConfigMode = false;
    loadSettings(); 
    registerNormalHandlers();
    server.begin();
}

void WifiApp::registerConfigHandlers() {
    server.on("/", []() { server.send(200, "text/html", wifi_config_html); });

    server.on("/scan", []() {
        int n = WiFi.scanNetworks();
        String json = "[";
        for (int i = 0; i < n; ++i) {
            if (i) json += ",";
            json += "\"" + WiFi.SSID(i) + "\"";
        }
        json += "]";
        server.send(200, "application/json", json); 
    });

    server.on("/save", HTTP_POST, [this]() {
        String s = server.arg("ssid");
        String p = server.arg("pass");
        String g = server.arg("gas");
        
        if (s.length() > 0) {
            preferences.putString("ssid", s);
            preferences.putString("pass", p);
            preferences.putString("gas", g);
            
            server.send(200, "text/plain", "Da luu! Dang khoi dong lai...");
            delay(1000);
            ESP.restart();
        } else {
            server.send(400, "text/plain", "Loi: Thieu ten Wifi");
        } 
    });

    server.on("/restart", []() {
        server.send(200, "text/plain", "Restarting...");
        delay(1000);
        ESP.restart(); 
    });
}

void WifiApp::registerNormalHandlers() {
    server.on("/", []() { server.send(200, "text/html", index_html); });

    server.on("/readings", []() {
        auto jsonFloat = [](float value, int digits) -> String {
            if (isnan(value) || isinf(value)) return "null";
            return String(value, (unsigned int)digits);
        };

        String json = "{";
        json += "\"temperature\":" + jsonFloat(temp, 1) + ",";
        json += "\"humidity\":" + jsonFloat(hum, 1) + ",";
        json += "\"voltage\":" + jsonFloat(inaBusVoltage, 2) + ",";
        json += "\"current\":" + jsonFloat(inaCurrent, 3) + ",";
        json += "\"ina226\":" + String(ina226Online ? 1 : 0) + ",";
        json += "\"inaError\":" + String(inaErrorCode) + ",";
        json += "\"inaBusRaw\":" + String(inaBusRaw) + ",";
        json += "\"inaShuntRaw\":" + String(inaShuntRaw) + ",";
        json += "\"inaManufacturer\":\"0x" + String(inaManufacturerId, HEX) + "\",";
        json += "\"inaDie\":\"0x" + String(inaDieId, HEX) + "\",";
        json += "\"fan\":" + String(fanState ? 1 : 0) + ",";
        json += "\"light\":" + String(lightState ? 1 : 0) + ",";
        json += "\"automode\":" + String(isAutoMode ? 1 : 0) + ",";
        json += "\"high\":" + String(tempHighLimit, 1) + ",";
        json += "\"low\":" + String(tempLowLimit, 1) + ",";
        json += "\"gas_id\":\"" + preferences.getString("gas", "") + "\"";
        json += "}";
        server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
        server.sendHeader("Pragma", "no-cache");
        server.send(200, "application/json", json); 
    });

    server.on("/savesettings", [this]() {
        bool updated = false;
        
        if (server.hasArg("h") && server.hasArg("l")) {
            float h = server.arg("h").toFloat();
            float l = server.arg("l").toFloat();
            if(h > l) {
                preferences.putFloat("tempHigh", h);
                preferences.putFloat("tempLow", l);
                tempHighLimit = h;
                tempLowLimit = l;
                updated = true;
            }
        }
        
        if (server.hasArg("gas")) {
            String g = server.arg("gas");
            preferences.putString("gas", g);
            updated = true;
        }

        if (server.hasArg("interval")) {
            long sec = server.arg("interval").toInt();
            if (sec >= 2) { 
                logInterval = sec * 1000;
                updated = true;
            }
        }

        if (updated) {
            server.send(200, "text/plain", "OK");
        } else {
            server.send(400, "text/plain", "Invalid or Missing args");
        }
    });

    server.on("/factoryreset", [this]() {
        server.send(200, "text/plain", "Factory reset in 3 seconds");
        Serial.println("Factory reset requested from web. Clearing preferences in 3 seconds...");

        for (int remaining = 3; remaining > 0; remaining--) {
            esp_task_wdt_reset();
            display.clearDisplay();
            display.setTextColor(WHITE);
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.println("Web reset");
            display.println("Xoa cau hinh...");
            display.print("Restart sau ");
            display.print(remaining);
            display.println("s");
            display.display();

            Serial.print("Factory reset countdown: ");
            Serial.println(remaining);
            delay(1000);
        }

        preferences.clear();
        WiFi.disconnect(true, true);
        delay(200);
        ESP.restart();
    });

    server.on("/togglelight", []() {
        if (server.hasArg("state")) {
            lightState = (server.arg("state").toInt() == 1);
        } else {
            lightState = !lightState;
        }
        server.send(200, "text/plain", lightState ? "ON" : "OFF");
    });

    server.on("/setmode", []() {
        if (server.hasArg("auto")) {
            isAutoMode = (server.arg("auto").toInt() == 1);
            server.send(200, "text/plain", isAutoMode ? "AUTO" : "MANUAL");
        } else {
            server.send(400, "text/plain", "Missing arg");
        }
    });

    server.on("/togglefan", []() {
        if (!isAutoMode) {
            if (server.hasArg("state")) {
                fanState = (server.arg("state").toInt() == 1);
            } else {
                fanState = !fanState;
            }
            server.send(200, "text/plain", fanState ? "ON" : "OFF");
        } else {
            server.send(400, "text/plain", "Mode is AUTO");
        }
    });
}

bool WifiApp::resetCheck() {
    if (isResetButtonPressed()) {
        unsigned long now = millis();
        if (!isResetPressing) {
            isResetPressing = true;
            resetPressStartTime = now;
            lastResetCountdown = -1;
            Serial.println("Reset button pressed, hold for 5 seconds...");
        }

        unsigned long elapsed = now - resetPressStartTime;
        unsigned long remaining = (elapsed >= RESET_HOLD_MS) ? 0 : ((RESET_HOLD_MS - elapsed + 999) / 1000);

        if ((int)remaining != lastResetCountdown) {
            lastResetCountdown = (int)remaining;
            Serial.print("Reset countdown: ");
            Serial.println(remaining);

            display.clearDisplay();
            display.setTextColor(WHITE);
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.println("Giu nut de reset");
            display.setTextSize(2);
            display.setCursor(20, 16);
            display.print(remaining);
            display.print("s");
            display.display();
        }

        if (elapsed >= RESET_HOLD_MS) {
            Serial.println("Reset confirmed. Clearing WiFi/settings...");
            display.clearDisplay();
            display.setTextColor(WHITE);
            display.setTextSize(2);
            display.setCursor(0, 8);
            display.print("RESET...");
            display.display();
            if (LED_PIN >= 0) digitalWrite(LED_PIN, HIGH);

            preferences.clear();
            delay(1000);
            ESP.restart();
        }

        return true;
    }

    if (isResetPressing) {
        Serial.println("Reset button released. Continue normal run.");
    }
    isResetPressing = false;
    lastResetCountdown = -1;
    if (LED_PIN >= 0) digitalWrite(LED_PIN, LOW);
    return false;
}

void WifiApp::sendToGoogleSheet() {
    if (millis() - lastLogTime < logInterval) return;
    lastLogTime = millis();

    String gas_id = preferences.getString("gas", "");
    if (gas_id.length() < 10) return; 

    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure();
        client.setTimeout(3000);
        
        HTTPClient https;
        https.setTimeout(3000);
        String intervalStr = String(logInterval / 1000) + "s";
        String tempStr = String(temp, 1);
        String humStr = String(hum, 1);
        String voltageStr = String(inaBusVoltage, 2);
        String currentStr = String(inaCurrent, 3);
        String url = "https://script.google.com/macros/s/" + gas_id
            + "/exec?temp=" + tempStr
            + "&hum=" + humStr
            + "&volt=" + voltageStr
            + "&voltage=" + voltageStr
            + "&curr=" + currentStr
            + "&current=" + currentStr
            + "&high=" + String(tempHighLimit)
            + "&low=" + String(tempLowLimit)
            + "&intv=" + intervalStr;

        Serial.print("GoogleSheet params temp=");
        Serial.print(tempStr);
        Serial.print(" hum=");
        Serial.print(humStr);
        Serial.print(" volt=");
        Serial.print(voltageStr);
        Serial.print(" curr=");
        Serial.println(currentStr);
        
        if (https.begin(client, url)) {
            int httpCode = https.GET();
            if (httpCode > 0) {
                Serial.printf("SEND OK: %d\n", httpCode);
            } else {
                Serial.printf("[HTTPS] Failed, error: %s\n", https.errorToString(httpCode).c_str());
            }
            https.end();
        } else {
            Serial.printf("[HTTPS] Failed to connect\n");
        }
    }
}
