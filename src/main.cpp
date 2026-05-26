#include <Arduino.h>
#include <TinyGPSPlus.h>

#if !defined(ESP32)
#include <SoftwareSerial.h>
#endif

#ifndef GPS_RX_PIN
#if defined(ESP32)
#define GPS_RX_PIN 16
#else
#define GPS_RX_PIN 0
#endif
#endif
#ifndef GPS_TX_PIN
#if defined(ESP32)
#define GPS_TX_PIN 17
#else
#define GPS_TX_PIN 2
#endif
#endif
#ifndef GPS_BAUD
#define GPS_BAUD 9600
#endif
#ifndef GPS_FEED_MS
#define GPS_FEED_MS 100
#endif

#if defined(ESP32)
// ESP32-C3 has UART0 + UART1 only (Serial / Serial1). Other ESP32 targets use Serial2.
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
HardwareSerial& gpsSerial = Serial1;
#else
HardwareSerial& gpsSerial = Serial2;
#endif
#else
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
#endif
TinyGPSPlus gps;

namespace {

void printFix() {
  Serial.print(F("Lat: "));
  Serial.print(gps.location.lat(), 6);
  Serial.print(F("  Lon: "));
  Serial.println(gps.location.lng(), 6);

  Serial.print(F("Alt: "));
  Serial.print(gps.altitude.meters());
  Serial.print(F(" m  Speed: "));
  Serial.print(gps.speed.kmph());
  Serial.println(F(" km/h"));

  Serial.print(F("Sats: "));
  Serial.print(gps.satellites.value());
  Serial.print(F("  HDOP: "));
  Serial.println(gps.hdop.hdop());

  if (gps.date.isValid() && gps.time.isValid()) {
    Serial.print(F("UTC: "));
    if (gps.date.day() < 10) Serial.print('0');
    Serial.print(gps.date.day());
    Serial.print('-');
    if (gps.date.month() < 10) Serial.print('0');
    Serial.print(gps.date.month());
    Serial.print('-');
    Serial.print(gps.date.year());
    Serial.print(' ');
    if (gps.time.hour() < 10) Serial.print('0');
    Serial.print(gps.time.hour());
    Serial.print(':');
    if (gps.time.minute() < 10) Serial.print('0');
    Serial.print(gps.time.minute());
    Serial.print(':');
    if (gps.time.second() < 10) Serial.print('0');
    Serial.println(gps.time.second());
  }

  Serial.println(F("---"));
}

void feedGps(unsigned long timeoutMs) {
  const unsigned long start = millis();
  while (millis() - start < timeoutMs) {
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
    yield();
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(100);

  // ESP8266: SoftwareSerial.begin(baud)
  // ESP32: HardwareSerial.begin(baud, config, rxPin, txPin)
#if defined(ESP32)
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
#else
  gpsSerial.begin(GPS_BAUD);
#endif
  gpsSerial.setTimeout(0);

  Serial.println();
  Serial.println(F("ESP-01 + u-blox NEO-8 GPS"));
  Serial.print(F("GPS UART: RX=GPIO"));
  Serial.print(GPS_RX_PIN);
  Serial.print(F(" TX=GPIO"));
  Serial.print(GPS_TX_PIN);
  Serial.print(F(" @ "));
  Serial.print(GPS_BAUD);
  Serial.println(F(" baud"));
  Serial.println(F("Waiting for satellite fix (outdoors / near window)..."));
}

void loop() {
  feedGps(GPS_FEED_MS);

  if (gps.location.isUpdated()) {
    printFix();
  } else if (!gps.location.isValid()) {
    Serial.print(F("No fix yet. Chars: "));
    Serial.print(gps.charsProcessed());
    Serial.print(F("  Sentences: "));
    Serial.print(gps.passedChecksum());
    Serial.print(F("  Failed: "));
    Serial.println(gps.failedChecksum());
  }
}
