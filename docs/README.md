# Board docs

Each file documents one PlatformIO `env:` target in `platformio.ini`, with:

- default GPS UART pins used by the firmware (`GPS_RX_PIN`, `GPS_TX_PIN`)
- wiring (GPS `TX`/`RX` cross)
- safe pins / boot-pin cautions
- quick build/upload/monitor commands

## Targets in this repo

- `esp01_1m`: `docs/esp01_1m.md`
- `esp32_doit_devkit_v1`: `docs/esp32_doit_devkit_v1.md`
- `nodemcu_32s`: `docs/nodemcu_32s.md`
- `nodemcu_esp32_s3`: `docs/nodemcu_esp32_s3.md`
