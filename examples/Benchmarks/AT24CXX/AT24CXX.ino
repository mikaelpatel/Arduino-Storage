/* Results:
 * ----------------------------------------
 * TWI@100kHz
 * ----------------------------------------
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
 *
 * ----------------------------------------
 * TWI@400kHz
 * ----------------------------------------
 * write(N, us, us/byte, kbyte/s)
 * 1, 132, 132.00, 7.58
 * 10, 352, 35.20, 28.41
 * 100, 12288, 122.88, 8.14
 * 1000, 125164, 125.16, 7.99
 *
 * read(N, us, us/byte, kbyte/s)
 * 1, 168, 168.00, 5.95
 * 10, 392, 39.20, 25.51
 * 100, 2640, 26.40, 37.88
 * 1000, 25152, 25.15, 39.76
 *
 * ----------------------------------------
 * TWI@800kHz
 * ----------------------------------------
 * write(N, us, us/byte, kbyte/s)
 * 1, 88, 88.00, 11.36
 * 10, 204, 20.40, 49.02
 * 100, 10916, 109.16, 9.16
 * 1000, 112656, 112.66, 8.88
 *
 * read(N, us, us/byte, kbyte/s)
 * 1, 104, 104.00, 9.62
 * 10, 228, 22.80, 43.86
 * 100, 1468, 14.68, 68.12
 * 1000, 13916, 13.92, 71.86
 */

#include "Storage.h"
#include "TWI.h"
#include "Driver/AT24CXX.h"

// Configure: TWI bus manager variant
// #define USE_SOFTWARE_TWI
#define USE_HARDWARE_TWI

// Configure: Hardware TWI bus clock frequency
// #define FREQ 800000UL
#define FREQ 400000UL
// #define FREQ 100000UL

#if defined(USE_SOFTWARE_TWI)
#include "GPIO.h"
#include "Software/TWI.h"
Software::TWI<BOARD::D18, BOARD::D19> twi;
#elif defined(USE_HARDWARE_TWI)
#include "Hardware/TWI.h"
Hardware::TWI twi(FREQ);
#endif

// EEPROM external storage
AT24C32 eeprom(twi);

// Local memory buffer
const int BUF_MAX = 1000;
uint8_t buf[BUF_MAX];

// Buffer sizes
const int N_MAX = 7;
const unsigned int N[N_MAX] = {
  1, 10,
  eeprom.PAGE_MAX, 2*eeprom.PAGE_MAX, 3*eeprom.PAGE_MAX,
  100, 1000
};

void setup()
{
  Serial.begin(57600);
  while (!Serial);
}

void loop()
{
  static uint8_t n = 0;
  uint32_t start;
  uint32_t us;
  float uspb;

  for (int i = 0; i < BUF_MAX; i++) buf[i] = n + i;

  Serial.println();
  Serial.print(F("twi.FREQ(kHz) = "));
  Serial.println(FREQ / 1000);
  Serial.print(F("eeprom.SIZE(kbyte) = "));
  Serial.println(eeprom.SIZE / 1024);
  Serial.println();

  // Benchmark#1: Measure write with increasing buffer size
  Serial.println(F("write(N, us, us/byte, kbyte/s)"));
  Serial.flush();
  for (int i = 0; i < N_MAX; i++) {
    start = micros();
    eeprom.write(0x0000, buf, N[i]);
    us = micros() - start;
    uspb = ((float) us) / N[i];
    Serial.print(N[i]);
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
  for (int i = 0; i < N_MAX; i++) {
    start = micros();
    eeprom.read(buf, 0x0000, N[i]);
    us = micros() - start;
    uspb = ((float) us) / N[i];
    Serial.print(N[i]);
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
