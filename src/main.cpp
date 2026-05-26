#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

#ifndef GPS_RX_PIN
#define GPS_RX_PIN 0
#endif
#ifndef GPS_TX_PIN
#define GPS_TX_PIN 2
#endif
#ifndef GPS_BAUD
#define GPS_BAUD 9600
#endif

SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
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

  gpsSerial.begin(GPS_BAUD);
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
  feedGps(1000);

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
