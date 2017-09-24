#include "Storage.h"
#include "Driver/EEPROM.h"

// Use internal eeprom for configuration storage
EEPROM eeprom;

// Configuration values
struct config_t {
  uint32_t timestamp;
  char ssid[32];
  uint8_t mac[6];
};

// Macro to enforce naming convention
#define PERSISTENT(v) persistent_ ## v

// Reset to default configuration flag
bool reset;
Storage::Cache PERSISTENT(reset)(eeprom, &reset, sizeof(reset));

// Persistent configuration
config_t config;
Storage::Cache PERSISTENT(config)(eeprom, &config, sizeof(config));

// Default configuration
static const char ssid[] PROGMEM = "Wi-Fi SSID";
static const uint8_t mac[6] PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  // Check the reset flag; write default configuration or read persistent
  PERSISTENT(reset).read();
  if (reset) {
    Serial.println(F("Write default configuration"));
    config.timestamp = 0;
    strcat_P(config.ssid, ssid);
    memcpy_P(config.mac, mac, sizeof(mac));
    PERSISTENT(config).write();
    reset = false;
    PERSISTENT(reset).write();
  }
  else {
    Serial.println(F("Read configuration"));
    PERSISTENT(config).read();
  }

  Serial.print(millis() / 1000.0);
  Serial.print(F(":reset@"));
  Serial.print(PERSISTENT(reset).addr());
  Serial.print(F(" = "));
  Serial.println(reset);
}

void loop()
{
  static uint8_t n = 0;
  uint32_t start = millis();

  // Print configuration
  Serial.print(start / 1000.0);
  Serial.print(F(":config@"));
  Serial.print(PERSISTENT(config).addr());
  Serial.print(F(": timestamp = "));
  Serial.print(config.timestamp);
  Serial.print(F(", ssid = \""));
  Serial.print(config.ssid);
  Serial.print(F("\", mac = "));
  for (size_t i = 0; i < sizeof(config.mac); i++) {
    Serial.print(config.mac[i], HEX);
    if (i + 1 < sizeof(config.mac)) Serial.print(':');
  }
  Serial.println();
  Serial.flush();
  uint32_t ms = millis() - start;
  delay(1000 - ms);
  if (++n < 60) return;
  n = 0;

  // Update timestamp every minute and write to eeprom
  Serial.println(F("Update timestamp"));
  config.timestamp++;
  PERSISTENT(config).Block::write(offsetof(config_t, timestamp),
				  &config.timestamp,
				  sizeof(config.timestamp));
}
