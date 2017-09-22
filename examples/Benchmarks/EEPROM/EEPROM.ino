/**
 * eeprom.SIZE: 1024
 *
 * write(N, us, us/byte, kbyte/s)
 * 1, 8, 8.00, 125.00
 * 10, 20, 2.00, 500.00
 * 100, 192, 1.92, 520.83
 * 1000, 1884, 1.88, 530.79
 *
 * read(N, us, us/byte, kbyte/s)
 * 1, 4, 4.00, 250.00
 * 10, 16, 1.60, 625.00
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

  Serial.print(F("eeprom.SIZE: "));
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

  delay(5000);
}
