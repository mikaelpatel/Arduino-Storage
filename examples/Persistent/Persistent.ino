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

// Reset to default configuration flag
bool reset;
Storage::Block persistent_reset(eeprom, &reset, sizeof(reset));

// Persistent configuration
config_t config;
Storage::Block persistent_config(eeprom, &config, sizeof(config));

// Default configuration
static const char ssid[] PROGMEM = "Wi-Fi SSID";
static const uint8_t mac[6] PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  // Check the reset flag; write default configuration or read persistent
  persistent_reset.read();
  if (reset) {
    Serial.println(F("Write default configuration"));
    config.timestamp = 0;
    strcat_P(config.ssid, ssid);
    memcpy_P(config.mac, mac, sizeof(mac));
    persistent_config.write();
    reset = false;
    persistent_reset.write();
  }
  else {
    Serial.println(F("Read configuration"));
    persistent_config.read();
  }

  Serial.print(F("reset@"));
  Serial.print(persistent_reset.addr());
  Serial.print(F(" = "));
  Serial.println(reset);
}

void loop()
{
  static uint8_t n = 0;

  // Print configuration
  Serial.print(F("config@"));
  Serial.print(persistent_config.addr());
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
  delay(1000);
  if (++n < 60) return;

  // Update timestamp every minute
  Serial.println(F("Update timestamp"));
  config.timestamp++;
  persistent_config.write();
  n = 0;
}
