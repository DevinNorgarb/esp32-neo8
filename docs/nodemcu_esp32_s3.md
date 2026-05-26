# NodeMCU ESP32-S3 + u-blox NEO-8 GPS

This doc matches PlatformIO target `env:nodemcu_esp32_s3` in `platformio.ini`.

Note: the code uses ESP32-S3 `Serial2` and remaps it to the GPIOs given by `GPS_RX_PIN` / `GPS_TX_PIN`.

## Firmware GPS UART defaults

- GPS baud (NMEA): `GPS_BAUD=9600`
- Firmware uses ESP32 `Serial2` remapped pins:
  - `GPS_RX_PIN=16` (ESP RX; connect to GPS `TX`)
  - `GPS_TX_PIN=17` (ESP TX; connect to GPS `RX`)

## Wiring

| NEO-8 | ESP32-S3 |
|------|-----------|
| `VCC` | `3.3V` |
| `GND` | `GND` |
| `TX` (GPS) | `GPIO GPS_RX_PIN` |
| `RX` (GPS) | `GPIO GPS_TX_PIN` |

## Picking other pins (common S3 cautions)

You can override `GPS_RX_PIN` / `GPS_TX_PIN` to use a different header row.

Avoid for normal projects:

- S3 strapping pins: `GPIO0`, `GPIO3`, `GPIO45`, `GPIO46`
- USB native pins (if your board uses native USB): `GPIO19`, `GPIO20`
- Flash/PSRAM pins on many S3 modules: `GPIO26`-`GPIO32` (and sometimes `GPIO33`-`GPIO37`)

## Build / upload / monitor

```bash
pio run -e nodemcu_esp32_s3
pio run -e nodemcu_esp32_s3 --target upload
pio device monitor -e nodemcu_esp32_s3
```

## Changing pins

Edit `env:nodemcu_esp32_s3` `build_flags`:

- `-D GPS_RX_PIN=<gpio>`
- `-D GPS_TX_PIN=<gpio>`

