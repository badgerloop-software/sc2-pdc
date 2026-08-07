# sc2-pdc

Firmware for Power Distribution and Controls on a Nucleo-L432KC.

## Build

1. Run `git submodule update --init`.
2. Run `pio run`.

## Modules

1. `main` starts the board, optional debug mode, and the CAN transmit loop.
2. `IOManagement` reads GPIO and ADC values. It writes accel and regen DAC outputs.
3. `speed_calc` counts MCU speed pulses and sets `rpm` and `mph`.
4. `motor_control` runs the PARK, IDLE, FORWARD, and REVERSE state machine.
5. `canPDC` receives driver inputs on CAN. It transmits telem on CAN.

## Build flags

Set these flags in `include/const.h`. Set `DEBUG_TECHNIQUE` in `main.cpp`.

| Flag | Role |
|------|------|
| `DEBUG_PRINTS` | Print status on Serial. This flag is on by default. |
| `TEST_MODE` | Use the CAN bounce test board path. |
| `DEBUG_TECHNIQUE` | `0` is production. `1` sends random CAN echo data. |

## Park brake

The park-brake sensor is not on the car. The firmware sets `park_brake` to false. The controller leaves PARK and runs.

## Pins

| Signal | Pin |
|--------|-----|
| Accel DAC | PA5 |
| Regen DAC | PA4 |
| Brake switch | PA0 (internal pulldown) |
| MCU speed pulse | PA8 |
| Direction | PB7 |
| Eco | PB1 |

## CAN map

Receive from the steering wheel:

- `0x300` direction
- `0x301` regen
- `0x302` throttle
- `0x303` eco or power mode

Transmit:

- `0x200` through `0x208`: accel out, regen, LV telem, brake, digital pack, mph
