#include "Storage.h"
#include "GPIO.h"
#include "SPI.h"
#include "Driver/MC23LCXXX.h"

// Configure: SPI bus manager implementation
#define USE_SOFTWARE_SPI

#if defined(USE_SOFTWARE_SPI)
#include "Software/SPI.h"
Software::SPI<BOARD::D11, BOARD::D12, BOARD::D13> spi;
#else
#include "Hardware/SPI.h"
Hardware::SPI spi;
#endif

// External memory device
MC23LC1024<BOARD::D10> sram(spi);

// Pin assignments are:
// SS D10, MOSI D11, MISO D12, SCK D13

// Local memory buffer
const int BUF_MAX = 1000;
static uint8_t buf[BUF_MAX];

void setup()
{
  Serial.begin(57600);
  while (!Serial);
}

void loop()
{
  static uint8_t n = 0;
  for (int i = 0; i < BUF_MAX; i++) buf[i] = n + i;

  Serial.println();
  Serial.println(F("spi.FREQ(MHz) = 8"));
  Serial.print(F("sram.SIZE(kbyte) = "));
  Serial.println(sram.SIZE / 1024);
  Serial.println();

  // Benchmark#1: Measure write with increasing buffer size
  Serial.println(F("write(N, us, us/byte, kbyte/s)"));
  Serial.flush();
  for (int i = 1; i < 10000; i *= 10) {
    uint32_t start = micros();
    sram.write(0x0000, buf, i);
    uint32_t us = micros() - start;
    float uspb = ((float) us) / i;
    Serial.print(i);
    Serial.print(F(", "));
    Serial.print(us);
    Serial.print(F(", "));
    Serial.print(uspb);
    Serial.print(F(", "));
    Serial.println(1000 / uspb);
    Serial.flush();
  }
  Serial.println();

  for (int i = 0; i < BUF_MAX; i++) buf[i] = 0;

  // Benchmark#2: Measure read with increasing buffer size
  Serial.println(F("read(N, us, us/byte, kbyte/s)"));
  Serial.flush();
  for (int i = 1; i < 10000; i *= 10) {
    uint32_t start = micros();
    sram.read(buf, 0x0000, i);
    uint32_t us = micros() - start;
    float uspb = ((float) us) / i;
    Serial.print(i);
    Serial.print(F(", "));
    Serial.print(us);
    Serial.print(F(", "));
    Serial.print(uspb);
    Serial.print(F(", "));
    Serial.println(1000 / uspb);
    Serial.flush();
  }

  for (int i = 0; i < BUF_MAX; i++) {
    if (buf[i] != ((n + i) & 0xff)) {
      Serial.print(i);
      Serial.println(F(": verification error"));
      break;
    }
  }
  n += 1;

  delay(5000);
}
