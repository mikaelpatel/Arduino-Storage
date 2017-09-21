#include "TWI.h"
#include "Storage.h"
#include "Driver/AT24CXX.h"

#define USE_SOFTWARE_TWI
// #define USE_HARDWARE_TWI

#if defined(USE_SOFTWARE_TWI)
#include "GPIO.h"
#include "Software/TWI.h"
#if defined(ARDUINO_attiny)
#include "Software/Serial.h"
Software::Serial<BOARD::D0> Serial;
Software::TWI<BOARD::D1, BOARD::D2> twi;
#else
Software::TWI<BOARD::D6, BOARD::D7> twi;
#endif
#elif defined(USE_HARDWARE_TWI)
#include "Hardware/TWI.h"
Hardware::TWI twi;
#endif

// EEPROM external storage
AT24C32 eeprom(twi);

// Address of data on storage
const uint32_t v_addr = eeprom.PAGE_MAX / 2;

// Local data
int16_t v[32];

// Block on storage for local data
Storage::Block v_eeprom(eeprom, v_addr, v, sizeof(v));

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  // Initiate vector (or read from eeprom)
#if 0
  Serial.println(F("setup:init"));
  for (size_t i = 0; i < sizeof(v) / sizeof(v[0]); i++) {
    v[i] = i;
  }
#else
  Serial.print(F("setup:read: "));
  Serial.println(v_eeprom.read());
#endif
}

void loop()
{
  // Print contents of vector and increment elements
  for (size_t i = 0; i < sizeof(v) / sizeof(v[0]); i++) {
    Serial.print(v[i]);
    Serial.print(' ');
    v[i] += 1;
  }
  Serial.println();
  delay(2000);

  // Write vector to eeprom
  Serial.print(F("loop:write: "));
  Serial.println(v_eeprom.write());

  // Clear vector to verify read
  for (size_t i = 0; i < sizeof(v) / sizeof(v[0]); i++) {
    v[i] = 0;
  }

  // Read vector from eeprom
  Serial.print(F("loop:read: "));
  Serial.println(v_eeprom.read());
}
