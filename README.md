# Arduino-Storage

The Storage library for Arduino is designed to abstract handling of
external memory, and allow block read/write and streaming of data. The
library includes device drivers for SPI SRAM (23LC512/1024), 2-Wire
EEPROM (AT24CXX) and internal EEPROM.

Device driver for external storage such as EEPROM and SRAM should
implement the Storage class virtual member functions and supply the
number of bytes on the device.

The Storage class supports linear allocation. The Storage::Block class
handles the allocated block and provides functions to read and write
to the block.

In many cases the external block contains data that mirrors data used
in the applications. The Storage::Cache handles read and write between
the block on external storage and data/variables. The Storage::Cache
class also allows handling of large vectors on external storage and
element/member access.

Version: 1.0

## Classes

* [Abstract Storage Manager, Storage](./src/Storage.h)
* [Storage Block, Storage::Block](./src/Storage.h)
* [Storage Cache, Storage::Cache](./src/Storage.h)
* [Storage Stream, Storage::Stream](./src/Storage.h)

## Drivers

* [2-Wire EEPROM, AT24CXX](./src/Driver/AT24CXX.h)
* [512 Kbit Serial SRAM, 23LC512](./src/Driver/MC23LC512.h)
* [1 Mbit Serial SRAM, 23LC1024](./src/Driver/MC23LC1024.h)
* [Internal EEPROM, EEPROM](./src/Driver/EEPROM.h)

## Example Sketches

* [Benchmarks](./examples/Benchmarks) measurement of characteristics.
* [Block](./examples/Block) read/write eeprom blocks.
* [Cache](./examples/Block) cache local variable.
* [Persistent](./examples/Persistent) read/write configuration.
* [Stream](./examples/Stream) storage as a print stream.
* [Vector](./examples/Vector) handling large sample vectors.

## Benchmarks

### AT24C32, 2-Wire EEPROM, 100 KHz
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

Note: 1) The read/write operations have a three byte header (8-bit
command, and 16-bit address). 2) Write performance increases up to
page size. When the block size is larger than the page size the write
operation will wait for the device to complete the page write. Typical
max 5-10 ms per page. 3) 400 KHz (and even 800 KHz) clock can be
used.

### 23LC1024, SPI SRAM, 8 MHz
#### Read
N | us | us/byte | kbyte/s
--|----|---------|--------
1 | 24 | 24.00 | 41.67
10 | 36 | 3.60 | 277.78
100 | 156 | 1.56 | 641.03
1000 | 1344 | 1.34 | 744.05

#### Write
N | us | us/byte | kbyte/s
--|----|---------|--------
1 | 24 | 24.00 | 41.67
10 | 36 | 3.60 | 277.78
100 | 172 | 1.72 | 581.40
1000 | 1468 | 1.47 | 681.20

Note: The read/write operations have a four byte header (8-bit
command, and 24-bit address).

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

Note: The internal eeprom has a single byte buffer and write of
multiple bytes will wait for the device to complete previous
write. Typical 3 ms.

## Dependencies

* [General Purpose Input/Output library for Arduino](https://github.com/mikaelpatel/Arduino-GPIO)
* [I2C Bus Manager and Device Driver Support](https://github.com/mikaelpatel/Arduino-TWI)
* [Serial Peripheral Interface Bus Manager and Device Driver Support](https://github.com/mikaelpatel/Arduino-SPI)