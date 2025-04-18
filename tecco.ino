#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// NRF24 Setup
RF24 radio(9, 10);  // CE, CSN
const byte address[6] = "fsam1";

// GPS Setup (Neo6M on D2/RX)
SoftwareSerial gpsSerial(2, 3);  // RX, TX
TinyGPSPlus gps;

// Battery pin
#define VOLTAGE_PIN A0

// Telemetry struct
struct TelemetryData {
  uint16_t packetID;
  float voltage;
  float latitude;
  float longitude;
};

TelemetryData dataToSend;
uint16_t packetCounter = 0;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  // Read battery voltage
  int analogValue = analogRead(VOLTAGE_PIN);
  dataToSend.voltage = analogValue * (5.0 / 1023.0) * (12.6 / 5.0);  // Adjust if voltage divider used

  // Parse GPS
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid()) {
    dataToSend.latitude = gps.location.lat();
    dataToSend.longitude = gps.location.lng();
  } else {
    dataToSend.latitude = 28.518317;
    dataToSend.longitude = 77.096956;
  }

  // Send telemetry
  dataToSend.packetID = packetCounter++;
  radio.write(&dataToSend, sizeof(dataToSend));

  delay(500);  // Adjust based on update rate
}