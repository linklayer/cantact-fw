# CANtact Firmware

[![Build Status](https://travis-ci.org/linklayer/cantact-fw.svg?branch=master)](https://travis-ci.org/linklayer/cantact-fw)

This repository contains sources for CANtact firmware.

Documentation can be found on the [Linklayer Wiki](https://wiki.linklayer.com/index.php/CANtact).

## STM32F072B-DISCO Notes

LD4:    "RED" led (can frame rx/tx)
LD5:    "Green" led (status)

CanBUS connection:
PB9:    `CAN_TX`
PB8:    `CAN_RX`

Firmware runs from internal HSI48 oscillator.
The connectors are nicely aligned together in top right of the board, on P2 connector.
GND, PB9, PB8, VCC is available in a single row.

Tested with: Waveshare CAN breakout board

USB connection via `USB USER` connector on the board

## Building

Firmware builds with GCC. Specifically, you will need gcc-arm-none-eabi, which
is packaged for Windows, OS X, and Linux on
[Launchpad](https://launchpad.net/gcc-arm-embedded/+download). Download for your
system and add the `bin` folder to your PATH.

With that done, you should be able to compile using `make`.

## Flashing & Debugging

Debugging and flashing can be done with any STM32 Discovery board as a
programmer. You can also use other tools that support SWD.

To use an STM32 Discovery, run [OpenOCD](http://openocd.sourceforge.net/) using
the stm32f0x.cfg file: `openocd -f fw/stm32f0x.cfg`.

With OpenOCD running, arm-none-eabi-gdb can be used to load code and debug.

## Contributors

- [Ethan Zonca](https://github.com/normaldotcom) - Makefile fixes and code size optimization
- [onejope](https://github.com/onejope) - Fixes to extended ID handling
- Phil Wise - Added dfu-util compatibility to Makefile

## License

See LICENSE.md
