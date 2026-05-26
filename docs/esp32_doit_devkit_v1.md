# DOIT ESP32 DevKit V1 + u-blox NEO-8 GPS

This doc matches PlatformIO target `env:esp32_doit_devkit_v1` in `platformio.ini`.

## Firmware GPS UART defaults

- GPS baud (NMEA): `GPS_BAUD=9600`
- Firmware uses ESP32 `Serial2` remapped pins:
  - `GPS_RX_PIN=16` (ESP RX; connect to GPS `TX`)
  - `GPS_TX_PIN=17` (ESP TX; connect to GPS `RX`)

## Wiring

| NEO-8 | ESP32 DOIT |
|------|-------------|
| `VCC` | `3.3V` |
| `GND` | `GND` |
| `TX` (GPS) | `GPIO16` (ESP RX) |
| `RX` (GPS) | `GPIO17` (ESP TX) |

## Build / upload / monitor

```bash
pio run -e esp32_doit_devkit_v1
pio run -e esp32_doit_devkit_v1 --target upload
pio device monitor -e esp32_doit_devkit_v1
```

## Changing pins

Update `GPS_RX_PIN` / `GPS_TX_PIN` in the `env:esp32_doit_devkit_v1` `build_flags`.

