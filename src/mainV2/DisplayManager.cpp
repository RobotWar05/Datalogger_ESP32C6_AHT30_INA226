#include "DisplayManager.h"

DisplayManager displayManager;

void DisplayManager::begin() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED error");
        oledReady = false;
        return;
    }

    oledReady = true;
    display.setTextColor(WHITE);
    showBoot();
}

bool DisplayManager::isReady() const {
    return oledReady;
}

void DisplayManager::drawCenteredText(const String &text, int16_t y, uint8_t textSize) {
    int16_t x1 = 0;
    int16_t y1 = 0;
    uint16_t w = 0;
    uint16_t h = 0;

    display.setTextSize(textSize);
    display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
    int16_t x = (SCREEN_WIDTH - (int16_t)w) / 2;
    if (x < 0) x = 0;
    display.setCursor(x, y);
    display.print(text);
}

void DisplayManager::drawCenteredValueLine(const String &text, int16_t y) {
    drawCenteredText(text, y, 2);
}

void DisplayManager::showBoot() {
    if (!oledReady) return;
    display.clearDisplay();
    display.setTextColor(WHITE);
    drawCenteredText("Khoi dong...", 12, 1);
    display.display();
}

void DisplayManager::showAhtError() {
    if (!oledReady) return;
    display.clearDisplay();
    display.setTextColor(WHITE);
    drawCenteredText("Loi AHT30!", 12, 1);
    display.display();
}

void DisplayManager::showWifiConnecting(const String &ssid, int remainingSeconds) {
    if (!oledReady) return;
    display.clearDisplay();
    display.setTextColor(WHITE);
    drawCenteredText("Ket noi WiFi", 0, 1);
    drawCenteredText(ssid, 11, 1);
    drawCenteredText("Timeout: " + String(remainingSeconds) + "s", 22, 1);
    display.display();
}

void DisplayManager::showWifiConnected(const IPAddress &ip) {
    if (!oledReady) return;
    display.clearDisplay();
    display.setTextColor(WHITE);
    drawCenteredText("WiFi OK", 5, 1);
    drawCenteredText(ip.toString(), 18, 1);
    display.display();
}

void DisplayManager::showWifiTimeout() {
    if (!oledReady) return;
    display.clearDisplay();
    display.setTextColor(WHITE);
    drawCenteredText("WiFi timeout!", 0, 1);
    drawCenteredText("Xoa WiFi cu", 11, 1);
    drawCenteredText("Restart...", 22, 1);
    display.display();
}

void DisplayManager::showConfigMode() {
    if (!oledReady) return;
    display.clearDisplay();
    display.setTextColor(WHITE);
    drawCenteredText("CHE DO CAU HINH", 0, 1);
    drawCenteredText("WiFi: HE THONG", 11, 1);
    drawCenteredText("IP: 192.168.4.1", 22, 1);
    display.display();
}

void DisplayManager::showFactoryResetCountdown(int remainingSeconds) {
    if (!oledReady) return;
    display.clearDisplay();
    display.setTextColor(WHITE);
    drawCenteredText("Web reset", 0, 1);
    drawCenteredText("Xoa cau hinh", 11, 1);
    drawCenteredText("Restart sau " + String(remainingSeconds) + "s", 22, 1);
    display.display();
}

void DisplayManager::showHardwareResetCountdown(unsigned long remainingSeconds) {
    if (!oledReady) return;
    display.clearDisplay();
    display.setTextColor(WHITE);
    drawCenteredText("Giu nut de reset", 2, 1);
    drawCenteredValueLine(String(remainingSeconds) + "s", 16);
    display.display();
}

void DisplayManager::showResetting() {
    if (!oledReady) return;
    display.clearDisplay();
    display.setTextColor(WHITE);
    drawCenteredValueLine("RESET...", 8);
    display.display();
}

void DisplayManager::updateRunScreen() {
    if (!oledReady) return;

    struct tm timeinfo;
    const bool timeSynced = getLocalTime(&timeinfo);

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);

    if (timeSynced) {
        char timeBuffer[6];
        snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        drawCenteredText(String(timeBuffer), 0, 1);
    } else {
        drawCenteredText("Dang cap nhat", 0, 1);
    }

    display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

    String line = "";
    if (isfinite(temp)) {
        line += String((int)temp);
    } else {
        line += "--";
    }
    line += "C  ";
    if (isfinite(hum)) {
        line += String((int)hum);
    } else {
        line += "--";
    }
    line += "%";
    drawCenteredValueLine(line, 15);

    display.display();
}
