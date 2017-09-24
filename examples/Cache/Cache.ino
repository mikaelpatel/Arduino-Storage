#include "Storage.h"
#include "TWI.h"
#include "Driver/AT24CXX.h"
#include "Driver/EEPROM.h"

// Configure: Force initialization of eeprom
// #define INITIATE_EEPROM

// Configure: TWI bus manager
#define USE_SOFTWARE_TWI
// #define USE_HARDWARE_TWI

// Configure: Hardware TWI bus clock frequency
#define FREQ 800000UL
// #define FREQ 400000UL
// #define FREQ 100000UL

#if defined(USE_SOFTWARE_TWI)
#include "GPIO.h"
#include "Software/TWI.h"
Software::TWI<BOARD::D18, BOARD::D19> twi;
#elif defined(USE_HARDWARE_TWI)
#include "Hardware/TWI.h"
Hardware::TWI twi(FREQ);
#endif

// Configure: EEPROM storage
AT24C32 eeprom(twi);
// EEPROM eeprom;

// Macro to enforce naming convention
#define CACHE(v) cache_ ## v

// Local data and storage cache block
int16_t v[32];
Storage::Cache CACHE(v)(eeprom, v, sizeof(v));

char w[32];
Storage::Cache CACHE(w)(eeprom, w, sizeof(w));

// Function to demonstrate dynamic allocation and caching
void calc()
{
  // Local cached variable with external storage
  int16_t u;
  Storage::Cache CACHE(u)(eeprom, &u, sizeof(u), 16);

  // Print cached variable; size, local and external memory address
  Serial.print(F("calc:eeprom.SIZE = "));
  Serial.print(eeprom.SIZE);
  Serial.print(F(", addr = "));
  Serial.println(eeprom.alloc(0));

  Serial.print(F("calc:CACHE(u).SIZE = "));
  Serial.print(CACHE(u).SIZE);
  Serial.print(F(", buf = "));
  Serial.print((int) &u);
  Serial.print(F(", addr = "));
  Serial.println(CACHE(u).addr());

  // Collect some data and write to external memory
  for (int i = 0; i < CACHE(u).NMEMB; i++) {
    u = analogRead(A0);
    CACHE(u).write(i);
  }
  Serial.println();

  // Read external memory and do some calculation
  int32_t sum = 0;
  for (int i = 0; i < CACHE(u).NMEMB; i++) {
    CACHE(u).read(i);
    Serial.print(u);
    Serial.print(' ');
    sum += u;
  }
  Serial.println();

  // Cached variable is deallocated from storage
}

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  // Initiate vector (or read from eeprom)
#if defined(INITIATE_EEPROM)
  Serial.println(F("setup:init"));
  for (size_t i = 0; i < sizeof(v) / sizeof(v[0]); i++) {
    v[i] = i;
  }
  strcat(w, "Nisse badar");
  CACHE(w).write();
#else
  Serial.print(F("setup:CACHE(v).read = "));
  Serial.println(CACHE(v).read());
#endif
  Serial.print(F("setup:CACHE(w).read = "));
  Serial.print(CACHE(w).read());
  Serial.print(F(", \""));
  Serial.print(w);
  Serial.println('"');
  Serial.println();
}

void loop()
{
  delay(5000);

  Serial.print(F("loop:eeprom.SIZE = "));
  Serial.print(eeprom.SIZE);
  Serial.print(F(", addr = "));
  Serial.println(eeprom.alloc(0));

  Serial.print(F("loop:CACHE(v).SIZE = "));
  Serial.print(CACHE(v).SIZE);
  Serial.print(F(", buf = "));
  Serial.print((int) &v);
  Serial.print(F(", addr = "));
  Serial.println(CACHE(v).addr());

  Serial.print(F("loop:CACHE(w).SIZE = "));
  Serial.print(CACHE(w).SIZE);
  Serial.print(F(", buf = "));
  Serial.print((int) &w);
  Serial.print(F(", addr = "));
  Serial.println(CACHE(w).addr());

  // Print contents of vector and increment elements
  for (size_t i = 0; i < sizeof(v) / sizeof(v[0]); i++) {
    Serial.print(v[i]);
    Serial.print(' ');
    v[i] += 1;
  }
  Serial.println();

  // Write vector to eeprom
  Serial.print(F("loop:CACHE(v).write = "));
  Serial.println(CACHE(v).write());

  // Call function that uses cached data on stack
  calc();

  // Clear vector to verify read
  for (size_t i = 0; i < sizeof(v) / sizeof(v[0]); i++) {
    v[i] = 0;
  }

  // Read vector from eeprom
  Serial.print(F("loop:CACHE(v).read = "));
  Serial.println(CACHE(v).read());
}
