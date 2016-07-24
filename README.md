# CANtact Firmware

[![Build Status](https://travis-ci.org/linklayer/cantact-fw.svg?branch=master)](https://travis-ci.org/linklayer/cantact-fw)

This repository contains sources for CANtact firmware.

## Building

Firmware builds with GCC. Specifically, you will need gcc-arm-none-eabi, which
is packaged for Windows, OS X, and Linux on
[Launchpad](https://launchpad.net/gcc-arm-embedded/+download). Download for your
system and add the `bin` folder to your PATH.

With that done, you should be able to compile using `make`. If you are compiling for a device that has no external crystal, compile with `make INTERNAL_OSCILLATOR=1`.

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
