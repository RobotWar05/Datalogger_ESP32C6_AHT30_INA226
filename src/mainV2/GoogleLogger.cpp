#include "GoogleLogger.h"

GoogleLogger googleLogger;

void GoogleLogger::update() {
    const unsigned long now = millis();
    if (now - lastLogTime < logIntervalMs) return;

    String gasId = preferences.getString("gas", "");
    if (gasId.length() < 10) return;
    if (WiFi.status() != WL_CONNECTED) return;

    if (!isfinite(temp) || !isfinite(hum)) {
        Serial.println("Skip GoogleSheet: AHT30 data is not valid yet");
        return;
    }

    lastLogTime = now;

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(3000);

    HTTPClient https;
    https.setTimeout(3000);

    const String intervalStr = String(logIntervalMs / 1000) + "s";
    const String tempStr = formatQueryFloat(temp, 1);
    const String humStr = formatQueryFloat(hum, 1);
    const String voltageStr = formatQueryFloat(inaBusVoltage, 2);
    const String currentStr = formatQueryFloat(inaCurrent, 3);

    String url = "https://script.google.com/macros/s/" + gasId
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
    Serial.print(voltageStr.length() ? voltageStr : "null");
    Serial.print(" curr=");
    Serial.println(currentStr.length() ? currentStr : "null");

    if (!https.begin(client, url)) {
        Serial.println("[HTTPS] Failed to connect");
        return;
    }

    const int httpCode = https.GET();
    if (httpCode > 0) {
        Serial.printf("SEND OK: %d\n", httpCode);
    } else {
        Serial.printf("[HTTPS] Failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
}

void GoogleLogger::setIntervalMs(unsigned long value) {
    if (value < 2000UL) value = 5000UL;
    logIntervalMs = value;
}

unsigned long GoogleLogger::getIntervalMs() const {
    return logIntervalMs;
}

String GoogleLogger::formatQueryFloat(float value, uint8_t digits) const {
    if (!isfinite(value)) return "";
    return String(value, (unsigned int)digits);
}
