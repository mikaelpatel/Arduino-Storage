# Arduino-Storage

The Storage library for Arduino is designed to abstract handling of
external memory, and allow block read/write and streaming of data. The
library includes device drivers for SPI SRAM (23LC1024), 2-Wire EEPROM
(AT24CXX) and internal EEPROM.

Version: 1.0

## Classes

* [Abstract Storage Manager, Storage](./src/Storage.h)
* [Storage Block, Storage::Block](./src/Storage.h)
* [Storage Stream, Storage::Stream](./src/Storage.h)

## Drivers

* [2-Wire EEPROM, AT24CXX](./src/Driver/AT24CXX.h)
* [1 Mbit Serial SRAM, 23LC1024](./src/Driver/MC23LC1024.h)
* [Internal EEPROM, EEPROM](./src/Driver/EEPROM.h)

## Example Sketches

* [Block](./examples/Block)
* [Storage](./examples/Storage)
* [Benchmarks](./examples/Benchmarks)

## Benchmarks

### AT24C32, 2-Wire EEPROM
#### Read
N | us | us/byte | kbyte/s
--|----|---------|--------
1 | 528 | 528.00 | 1.89
10 | 1352 | 135.20 | 7.40
100 | 9680 | 96.80 | 10.33
1000 | 93128 | 93.13 | 10.74
#### Write
N | us | us/byte | kbyte/s
--|----|---------|--------
1 | 408 | 408.00 | 2.45
10 | 1236 | 123.60 | 8.09
100 | 20540 | 205.40 | 4.87
1000 | 206280 | 206.28 | 4.85

### 23LC1024, SPI SRAM
#### Read
N | us | us/byte | kbyte/s
--|----|---------|--------
1 | 24 | 24.00 | 41.67
10 | 40 | 4.00 | 250.00
100 | 172 | 1.72 | 581.40
1000 | 1532 | 1.53 | 652.74
#### Write
N | us | us/byte | kbyte/s
--|----|---------|--------
1 | 24 | 24.00 | 41.67
10 | 36 | 3.60 | 277.78
100 | 172 | 1.72 | 581.40
1000 | 1468 | 1.47 | 681.20

### EEPROM
#### Read
N | us | us/byte | kbyte/s
--|----|---------|--------
1 | 4 | 4.00 | 250.00
10 | 16 | 1.60 | 625.00
100 | 112 | 1.12 | 892.86
1000 | 1072 | 1.07 | 932.84

#### Write
N | us | us/byte | kbyte/s
--|----|---------|--------
1 | 4 | 4.00 | 250.00
10 | 30780 | 3078.00 | 0.32
100 | 308140 | 3081.40 | 0.32
1000 | 3077476 | 3077.48 | 0.32

## Dependencies

* [General Purpose Input/Output library for Arduino](https://github.com/mikaelpatel/Arduino-GPIO)
* [I2C Bus Manager and Device Driver Support](https://github.com/mikaelpatel/Arduino-TWI)
* [Serial Peripheral Interface Bus Manager and Device Driver Support](https://github.com/mikaelpatel/Arduino-SPI)