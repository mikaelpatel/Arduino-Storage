/**
 * eeprom.SIZE(bytes): 1024
 *
 * write(N, us, us/byte, kbyte/s)
 * 1, 4, 4.00, 250.00
 * 10, 30780, 3078.00, 0.32
 * 100, 308140, 3081.40, 0.32
 * 1000, 3077476, 3077.48, 0.32
 *
 * read(N, us, us/byte, kbyte/s)
 * 1, 4, 4.00, 250.00
 * 10, 12, 1.20, 833.33
 * 100, 112, 1.12, 892.86
 * 1000, 1072, 1.07, 932.84
 */

#include "Storage.h"
#include "Driver/EEPROM.h"

// EEPROM internal storage
EEPROM eeprom;

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

  Serial.print(F("eeprom.SIZE(bytes): "));
  Serial.println(eeprom.SIZE);
  Serial.println();

  // Benchmark#1: Measure write with increasing buffer size
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

  // Benchmark#2: Measure read with increasing buffer size
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
