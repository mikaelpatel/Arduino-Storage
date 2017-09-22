/*
 * eeprom.SIZE = 4096
 *
 * write(N, us, us/byte, kbyte/s)
 * 1, 408, 408.00, 2.45
 * 10, 1236, 123.60, 8.09
 * 100, 20540, 205.40, 4.87
 * 1000, 206280, 206.28, 4.85
 *
 * read(N, us, us/byte, kbyte/s)
 * 1, 528, 528.00, 1.89
 * 10, 1352, 135.20, 7.40
 * 100, 9680, 96.80, 10.33
 * 1000, 93128, 93.13, 10.74
 */

#include "Storage.h"
#include "TWI.h"
#include "Driver/AT24CXX.h"

// #define USE_SOFTWARE_TWI
#define USE_HARDWARE_TWI

#if defined(USE_SOFTWARE_TWI)
#include "GPIO.h"
#include "Software/TWI.h"
Software::TWI<BOARD::D6, BOARD::D7> twi;
#elif defined(USE_HARDWARE_TWI)
#include "Hardware/TWI.h"
Hardware::TWI twi;
#endif

// EEPROM external storage
AT24C32 eeprom(twi);

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

  Serial.print(F("eeprom.SIZE = "));
  Serial.println(eeprom.SIZE);
  Serial.println();
  Serial.println(F("write(N, us, us/byte, kbyte/s)"));
  Serial.flush();
  for (int i = 1; i < 10000; i *= 10) {
    uint32_t start = micros();
    eeprom.write(0x0000, buf, i);
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

  Serial.println(F("read(N, us, us/byte, kbyte/s)"));
  Serial.flush();
  for (int i = 1; i < 10000; i *= 10) {
    uint32_t start = micros();
    eeprom.read(buf, 0x0000, i);
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
