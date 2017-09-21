#include "TWI.h"
#include "Storage.h"
#include "Driver/EEPROM.h"
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
// AT24C32 eeprom(twi);
EEPROM eeprom;

// Local data and storage block
int16_t v[32];
Storage::Block v_eeprom(eeprom, v, sizeof(v));

char w[32];
Storage::Block w_eeprom(eeprom, w, sizeof(w));

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  Serial.print(F("setup:v_eeprom.addr: "));
  Serial.println(v_eeprom.addr());

  Serial.print(F("setup:w_eeprom.addr: "));
  Serial.println(w_eeprom.addr());

  Serial.print(F("setup:eeprom.addr: "));
  Serial.println(eeprom.alloc(0));

  // Initiate vector (or read from eeprom)
#if 0
  Serial.println(F("setup:init"));
  for (size_t i = 0; i < sizeof(v) / sizeof(v[0]); i++) {
    v[i] = i;
  }
  strcat(w, "Nisse badar");
  w_eeprom.write();
#else
  Serial.print(F("setup:read: "));
  Serial.println(v_eeprom.read());
#endif
  Serial.print(F("setup:w_eeprom.addr: "));
  Serial.print(w_eeprom.read());
  Serial.print(F(", \""));
  Serial.print(w);
  Serial.println('"');
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
