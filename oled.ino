#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float voltage = 0.0;
int rssi = 0;

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true); // OLED failed
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Telemetry Booting...");
  display.display();
  delay(1000);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    parseData(input);
    updateDisplay();
  }
}

void parseData(String data) {
  int vIndex = data.indexOf("V:");
  int rIndex = data.indexOf("R:");
  if (vIndex >= 0 && rIndex >= 0) {
    voltage = data.substring(vIndex + 2, data.indexOf(",", vIndex)).toFloat();
    rssi = data.substring(rIndex + 2).toInt();
  }
}

void drawSignalBars(int level) {
  int baseX = 100;
  int baseY = 10;
  int barWidth = 4;
  for (int i = 0; i < 4; i++) {
    int height = (i + 1) * 3;
    if (i < level) {
      display.fillRect(baseX + i * (barWidth + 2), baseY - height, barWidth, height, SSD1306_WHITE);
    } else {
      display.drawRect(baseX + i * (barWidth + 2), baseY - height, barWidth, height, SSD1306_WHITE);
    }
  }
}

void drawBatteryIcon(float voltage) {
  int x = 100;
  int y = 40;
  int level = map(voltage * 10, 105, 126, 0, 5); // scale 10.5V–12.6V to 0–5
  if (level < 0) level = 0;
  if (level > 5) level = 5;

  // Battery outer frame
  display.drawRect(x, y, 22, 10, SSD1306_WHITE);
  display.fillRect(x + 22, y + 3, 2, 4, SSD1306_WHITE); // battery terminal

  // Battery level fill
  for (int i = 0; i < level; i++) {
    display.fillRect(x + 2 + i * 4, y + 2, 3, 6, SSD1306_WHITE);
  }
}

void updateDisplay() {
  display.clearDisplay();

  // Title
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("UAV Telemetry");

  // Voltage Text
  display.setCursor(0, 20);
  display.setTextSize(2);
  display.print("V:");
  display.print(voltage, 1);
  display.print("V");

  // RSSI Text
  display.setCursor(0, 42);
  display.setTextSize(2);
  display.print("S:");
  display.print(rssi);
  display.print("%");

  // Draw signal bars (based on RSSI)
  int bars = map(rssi, 0, 100, 0, 4);
  drawSignalBars(bars);

  // Draw battery icon
  drawBatteryIcon(voltage);

  // Alerts
  display.setTextSize(1);
  if (voltage < 10.5) {
    display.setCursor(0, 60);
    display.print("!! LOW BATTERY !!");
  } else if (rssi < 40) {
    display.setCursor(0, 60);
    display.print("!! WEAK SIGNAL !!");
  }

  display.display();
}
