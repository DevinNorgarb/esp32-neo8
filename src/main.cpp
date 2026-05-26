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
#if defined(ESP32) && !defined(GPS_NAV_HZ)
#define GPS_NAV_HZ 10
#elif !defined(GPS_NAV_HZ)
#define GPS_NAV_HZ 1
#endif
#ifndef GPS_STATUS_MS
#define GPS_STATUS_MS 1000
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

void beginGpsSerial(uint32_t baud) {
#if defined(ESP32)
  gpsSerial.begin(baud, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
#else
  (void)baud;
  gpsSerial.begin(baud);
#endif
  gpsSerial.setTimeout(0);
}

void flushGpsRx() {
  while (gpsSerial.available() > 0) {
    gpsSerial.read();
  }
}

void sendUbx(uint8_t msgClass, uint8_t msgId, const uint8_t* payload, uint16_t len) {
  uint8_t ckA = 0;
  uint8_t ckB = 0;
  auto checksum = [&](uint8_t b) {
    ckA = static_cast<uint8_t>(ckA + b);
    ckB = static_cast<uint8_t>(ckB + ckA);
  };

  gpsSerial.write(0xB5);
  gpsSerial.write(0x62);
  gpsSerial.write(msgClass);
  checksum(msgClass);
  gpsSerial.write(msgId);
  checksum(msgId);
  gpsSerial.write(static_cast<uint8_t>(len & 0xFF));
  checksum(static_cast<uint8_t>(len & 0xFF));
  gpsSerial.write(static_cast<uint8_t>(len >> 8));
  checksum(static_cast<uint8_t>(len >> 8));
  for (uint16_t i = 0; i < len; ++i) {
    gpsSerial.write(payload[i]);
    checksum(payload[i]);
  }
  gpsSerial.write(ckA);
  gpsSerial.write(ckB);
  gpsSerial.flush();
}

// Raise measurement rate only — do not change UART baud (breaks RX if CFG-PRT fails).
void configureGpsNavRate() {
  const uint16_t measMs = static_cast<uint16_t>(1000 / GPS_NAV_HZ);
  const uint8_t rate[] = {
      static_cast<uint8_t>(measMs & 0xFF),
      static_cast<uint8_t>(measMs >> 8),
      0x01,
      0x00,
      0x00,
      0x00,
  };
  sendUbx(0x06, 0x08, rate, sizeof(rate));
}

bool listenForNmea(uint32_t baud, unsigned long listenMs) {
  beginGpsSerial(baud);
  flushGpsRx();

  TinyGPSPlus probe;
  const unsigned long start = millis();
  while (millis() - start < listenMs) {
    while (gpsSerial.available() > 0) {
      if (probe.encode(gpsSerial.read()) && probe.passedChecksum() > 0) {
        return true;
      }
    }
    yield();
  }
  return false;
}

uint32_t detectGpsBaud() {
  const uint32_t candidates[] = {GPS_BAUD, 9600, 38400, 115200};
  for (uint32_t baud : candidates) {
    if (listenForNmea(baud, 800)) {
      return baud;
    }
  }
  return GPS_BAUD;
}

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

void drainGps() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
}

void printNoFixStatus() {
  Serial.print(F("No fix yet. Chars: "));
  Serial.print(gps.charsProcessed());
  Serial.print(F("  Sentences: "));
  Serial.print(gps.passedChecksum());
  Serial.print(F("  Failed: "));
  Serial.println(gps.failedChecksum());

  if (gps.passedChecksum() == 0) {
    Serial.println(F("No NMEA parsed — check GPS TX -> ESP RX and baud (9600)."));
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println(F("ESP-01 + u-blox NEO-8 GPS"));
  Serial.print(F("GPS UART: RX=GPIO"));
  Serial.print(GPS_RX_PIN);
  Serial.print(F(" TX=GPIO"));
  Serial.println(GPS_TX_PIN);

  const uint32_t uartBaud = detectGpsBaud();
  beginGpsSerial(uartBaud);
  flushGpsRx();

  if (listenForNmea(uartBaud, 500)) {
    Serial.print(F("NMEA OK @ "));
    Serial.print(uartBaud);
    Serial.println(F(" baud"));
#if GPS_NAV_HZ > 1
    configureGpsNavRate();
    delay(200);
    flushGpsRx();
#endif
  } else {
    Serial.println(F("WARNING: No NMEA — GPS TX must reach ESP RX (GPIO4 on C3)."));
  }

  Serial.print(F("Navigation rate target: "));
  Serial.print(GPS_NAV_HZ);
  Serial.println(F(" Hz"));
  Serial.println(F("Waiting for satellite fix (outdoors / near window)..."));
}

void loop() {
  drainGps();

  if (gps.location.isUpdated()) {
    printFix();
  } else if (!gps.location.isValid()) {
    static unsigned long lastStatusMs = 0;
    const unsigned long now = millis();
    if (now - lastStatusMs >= GPS_STATUS_MS) {
      lastStatusMs = now;
      printNoFixStatus();
    }
  }
}
