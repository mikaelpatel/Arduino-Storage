#include "Storage.h"
#include "GPIO.h"
#include "SPI.h"
#include "Driver/MC23LC1024.h"

// #define USE_SOFTWARE_SPI
#define USE_HARDWARE_SPI

#if defined(USE_SOFTWARE_SPI)
#include "Software/SPI.h"
#if defined(ARDUINO_attiny)
Software::SPI<BOARD::D1, BOARD::D2, BOARD::D3> spi;
#else
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
#endif
#elif defined(USE_HARDWARE_SPI)
#include "Hardware/SPI.h"
Hardware::SPI spi;
#endif

// External memory device
MC23LC1024<BOARD::D10> sram(spi);

// Local memory buffer
const size_t BUF_MAX = 1000;
static uint8_t buf[BUF_MAX];

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  for (size_t i = 0; i < BUF_MAX; i++) buf[i] = i;
}

void loop()
{
  // SRAM performance: N is number of bytes
  // write: 9.875 + N*1.4275 us, max. 700 kbyte/s
  // read: 9.875 + N*1.5 us, max. 670 kbyte/s

  // 9.875/9.875 us, protocol overhead
  sram.write(0x0000, buf, 0);
  sram.read(buf, 0x0000, 0);

  // 11.56/11.62 us/byte, 90 kbyte/s, 1 byte
  sram.write(0x0001, buf, 1);
  sram.read(buf, 0x0001, 1);

  // 2.45/2.49 us/byte, 410 kbyte/s, 10 byte
  sram.write(0x0010, buf, 10);
  sram.read(buf, 0x0010, 10);

  // 1.539/1.599 us/byte, 650 kbyte/s, 100 byte
  sram.write(0x0100, buf, 100);
  sram.read(buf, 0x00100, 100);

  // 1.454/1.522 us/byte, 690 kbyte/s, 1000 byte
  sram.write(0x1000, buf, sizeof(buf));
  sram.read(buf, 0x1000, sizeof(buf));

  // Print buffer and increment values
  for (size_t i = 0; i < BUF_MAX; i++) {
    Serial.print(buf[i]);
    Serial.print(' ');
    buf[i] = buf[i] + 1;
  }
  Serial.println();
  delay(500);
}
