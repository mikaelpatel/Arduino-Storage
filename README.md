# Arduino-Storage

The External Memory Storage library for Arduino is designed to
abstract handling of external memory, and allow block read/write and
streaming of data. The library includes device drivers for SPI SRAM
(23LC1024), 2-Wire EEPROM (AT24CXX) and internal EEPROM.

Version: 1.0

## Classes

* [Abstract Storage Manager, Storage](./src/Storage.h)
* [Storage Block, Storage::Block](./src/Storage.h)
* [Storage Stream, Storage::Stream](./src/Storage.h)

## Drivers

* [1 Mbit Serial SRAM, 23LC1024](./src/Driver/MC23LC1024.h)
* [2-Wire EEPROM, AT24CXX](./src/Driver/AT24CXX.h)
* [Internal EEPROM, EEPROM](./src/Driver/EEPROM.h)

## Example Sketches

* [Block](./examples/Block)
* [Storage](./examples/Storage)
* [Stream](./examples/Stream)

## Dependencies

* [General Purpose Input/Output library for Arduino](https://github.com/mikaelpatel/Arduino-GPIO)
* [I2C Bus Manager and Device Driver Support](https://github.com/mikaelpatel/Arduino-TWI)
* [Serial Peripheral Interface Bus Manager and Device Driver Support](https://github.com/mikaelpatel/Arduino-SPI)