# ESP-01 (ESP8266) + u-blox NEO-8 GPS

This doc matches PlatformIO target `env:esp01_1m` in `platformio.ini`.

## Firmware GPS UART defaults

- GPS baud (NMEA): `GPS_BAUD=9600`
- Firmware uses `SoftwareSerial`:
  - `GPS_RX_PIN=0` (ESP RX pin; connect to GPS `TX`)
  - `GPS_TX_PIN=2` (ESP TX pin; connect to GPS `RX`)

## Wiring

| NEO-8 | ESP-01 |
|------|--------|
| `VCC` | `3.3V` |
| `GND` | `GND` |
| `TX` (GPS) | `GPIO0` (ESP RX) |
| `RX` (GPS) | `GPIO2` (ESP TX) |

## Boot/power notes (important for ESP-01)

- `GPIO0` affects boot mode. Use a proper pull-up (typical ESP-01 devboards already include it).
- Use a stable `3.3V` supply; GPS modules can draw noticeable current during acquisition.
- Ensure `CH_PD/EN` is high (most ESP-01 dev boards tie it correctly).

## Build / upload / monitor

```bash
pio run -e esp01_1m
pio run -e esp01_1m --target upload
pio device monitor -e esp01_1m
```

## If you need different pins

Override in `platformio.ini` (or build flags) by changing:

- `-D GPS_RX_PIN=<gpio_rx>`
- `-D GPS_TX_PIN=<gpio_tx>`

