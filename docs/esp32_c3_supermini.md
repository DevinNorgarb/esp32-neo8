# ESP32-C3 Super Mini + u-blox NEO-8 GPS

This doc matches PlatformIO target `env:esp32_c3_supermini` in `platformio.ini`.

The Super Mini has no first-class PlatformIO board definition; firmware targets `esp32-c3-devkitm-1` with USB-CDC flags for serial over USB-C.

## Firmware GPS UART defaults

- GPS baud (NMEA): `GPS_BAUD=9600`
- Firmware uses ESP32-C3 `Serial1` remapped pins:
  - `GPS_RX_PIN=4` (ESP RX; connect to GPS `TX`)
  - `GPS_TX_PIN=5` (ESP TX; connect to GPS `RX`)

## Wiring (board silk labels)

| NEO-8 | ESP32-C3 Super Mini |
|------|----------------------|
| `VCC` | `3.3V` |
| `GND` | `GND` |
| `TX` (GPS) | `IO4` / GPIO4 (ESP RX) |
| `RX` (GPS) | `IO5` / GPIO5 (ESP TX) |

## Pin cautions

- **Do not** use `GPIO20` / `GPIO21` (`RX` / `TX` on the header) for GPS — those are UART0 and are tied to USB serial / flashing on many boards.
- **Avoid strapping pins** for peripherals: `GPIO2`, `GPIO8` (onboard LED), `GPIO9` (BOOT button).
- `GPIO4`–`GPIO7` can be used for JTAG debugging; leave them free if you use a debugger.

## Build / upload / monitor

```bash
pio run -e esp32_c3_supermini
pio run -e esp32_c3_supermini --target upload
pio device monitor -e esp32_c3_supermini
```

If the serial monitor is blank after upload, confirm `ARDUINO_USB_CDC_ON_BOOT=1` is set in `build_flags` (already enabled for this env).

## Changing pins

Update `GPS_RX_PIN` / `GPS_TX_PIN` in the `env:esp32_c3_supermini` `build_flags`.
