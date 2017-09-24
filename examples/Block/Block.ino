#include "Storage.h"
#include "TWI.h"
#include "Driver/AT24CXX.h"
#include "Driver/EEPROM.h"

// Configure: TWI bus manager
#define USE_SOFTWARE_TWI
// #define USE_HARDWARE_TWI

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

// Configure: EEPROM storage
AT24C32 eeprom(twi);
// EEPROM eeprom;

const uint32_t BLOCK_MAX = 16*eeprom.PAGE_MAX;
Storage::Block block(eeprom, BLOCK_MAX);

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  // Initiate eeprom; fill block with zero
  uint8_t page[eeprom.PAGE_MAX];
  memset(page, 0, sizeof(page));
  for (uint32_t offset = 0; offset < BLOCK_MAX; offset += eeprom.PAGE_MAX) {
    block.write(offset, page, sizeof(page));
  }
}

void loop()
{
  uint8_t page[eeprom.PAGE_MAX];
  for (uint32_t offset = 0; offset < BLOCK_MAX; offset += eeprom.PAGE_MAX) {
    // Read page buffer from block storage
    block.read(page, offset, sizeof(page));
    // Print page contents and update
    if (offset < 0x10) Serial.print('0');
    if (offset < 0x100) Serial.print('0');
    Serial.print(offset, HEX);
    Serial.print(':');
    for (size_t i = 0; i < sizeof(page); i++) {
      uint8_t v = page[i];
      Serial.print(' ');
      if (v < 0x10) Serial.print('0');
      Serial.print(v, HEX);
      page[i] = v + 1;
    }
    Serial.println();
    // Write page back to block storage
    block.write(offset, page, sizeof(page));
  }
  Serial.println();
  delay(5000);
}
