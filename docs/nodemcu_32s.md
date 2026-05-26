# NodeMCU-32S (ESP32) + u-blox NEO-8 GPS

This doc matches PlatformIO target `env:nodemcu_32s` in `platformio.ini`.

## Firmware GPS UART defaults

- GPS baud (NMEA): `GPS_BAUD=9600`
- Firmware uses ESP32 `Serial2` remapped pins:
  - `GPS_RX_PIN=25` (ESP RX; connect to GPS `TX`)
  - `GPS_TX_PIN=26` (ESP TX; connect to GPS `RX`)

## Wiring (using the pin labels on your board)

| NEO-8 | NodeMCU-32S |
|------|--------------|
| `VCC` | `3.3V` |
| `GND` | `GND` |
| `TX` (GPS) | `P25` (GPIO25; ESP RX) |
| `RX` (GPS) | `P26` (GPIO26; ESP TX) |

## Important boot / serial cautions (NodeMCU-32S)

- Avoid using the USB serial pins `TX=GPIO1` / `RX=GPIO3` for GPS (those are needed for flashing/monitor).
- Avoid boot-strapping pins on the ESP32 unless you *know* what you are doing:
  - `GPIO0` (`P0`), `GPIO2` (`P2`), `GPIO12` (`P12`), `GPIO15` (`P15`)

## Build / upload / monitor

```bash
pio run -e nodemcu_32s
pio run -e nodemcu_32s --target upload
pio device monitor -e nodemcu_32s
```

## Changing pins

Update `GPS_RX_PIN` / `GPS_TX_PIN` in the `env:nodemcu_32s` `build_flags`.

