#include "Storage.h"
#include "GPIO.h"
#include "SPI.h"
#include "Driver/MC23LC1024.h"

// Select SPI bus manager implementation
// #define USE_SOFTWARE_SPI
#define USE_HARDWARE_SPI

#if defined(USE_SOFTWARE_SPI)
#include "Software/SPI.h"
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
#elif defined(USE_HARDWARE_SPI)
#include "Hardware/SPI.h"
Hardware::SPI spi;
#endif

// External memory device
MC23LC1024<BOARD::D10> sram(spi);

// Pin assignments are:
// SS D10, MOSI D11, MISO D12, SCK D13

// Local memory buffer
const size_t BUF_MAX = 1000;
static uint8_t buf[BUF_MAX];

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  Serial.print(F("sram.SIZE = "));
  Serial.println(sram.SIZE);
  Serial.flush();
}

void loop()
{
  // SRAM performance: N is number of bytes
  // protocol: acquire/release + header, max 22 us
  // acquire/release: 11.38 us
  // header: 9.875 us
  // write: protocol + N*1.4275 us, max. 700 kbyte/s
  // read: protocol + N*1.5 us, max. 667 kbyte/s

  // 9.875/9.875 us, header, 4 bytes
  sram.write(0x0000, buf, 0);
  sram.read(buf, 0x0000, 0);

  // transfer: 11.56/11.62 us/byte, 90 kbyte/s, 1 byte
  // total: 23 us, 43 kbyte/s, 1+4 byte
  sram.write(0x0001, buf, 1);
  sram.read(buf, 0x0001, 1);

  // transfer: 2.45/2.49 us/byte, 410 kbyte/s, 10 byte
  // total: 36 us, 280 kbyte/s, 10+4 byte
  sram.write(0x0010, buf, 10);
  sram.read(buf, 0x0010, 10);

  // transfer: 1.539/1.599 us/byte, 650 kbyte/s, 100 byte
  // total: 165/170 us, 600 kbyte/s, 100+4 byte
  sram.write(0x0100, buf, 100);
  sram.read(buf, 0x00100, 100);

  // transfer: 1.454/1.522 us/byte, 690 kbyte/s, 1000 byte
  // total: 1470/1530 us, 667 kbyte/s, 1000+4 byte
  sram.write(0x1000, buf, sizeof(buf));
  sram.read(buf, 0x1000, sizeof(buf));

  delay(1000);
}
