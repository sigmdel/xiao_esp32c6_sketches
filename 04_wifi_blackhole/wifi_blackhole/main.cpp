// Main module of wifi_blackhole
// Copyright: see notice in wifi_blackhole.ino

#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"

unsigned long connect_time = 0;
unsigned long etime = 0;
unsigned long itime = 0;

#if defined(ARDUINO_MAKERGO_C3_SUPERMINI)
  #define TITLE "MAKERGO_C3_SUPERMINI"
  #define NEED_TX_POWER             // no name super mini C3 with antenna issues
#elif defined(ARDUINO_NOLOGO_ESP32C3_SUPER_MINI)
  #define TITLE "NOLOGO_ESP32C3_SUPER_MINI"
#elif defined(ARDUINO_DFROBOT_BEETLE_ESP32_C3)
  #define TITLE "DFROBOT_BEETLE_ESP32_C3"  
#elif defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "XIAO_ESP32C3"
#elif defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  // Uncomment the following macro to use a connected external antenna.
  //#define USE_EXTERNAL_ANTENNA
  #define TITLE "XIAO_ESP32C6"
#elif defined(ARDUINO_LOLIN_S2_MINI) 
  #define TITLE "LOLIN_S2_MINI"   
#elif defined(ARDUINO_LOLIN32_LITE)
  #define TITLE "LOLIN32_LITE"   
  #define BAUD 115200
#elif defined(ARDUINO_LILYGO_T_DISPLAY)  
  #define TITLE "LILYGO_T_DISPLAY"
  #define BAUD 115200
#else
  #error "Unknown board"
#endif  


#ifdef NEED_TX_POWER
  #define TX_POWER WIFI_POWER_11dBm
    // Set the above macro to the power level. See
    // https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFiGeneric.h
#endif


void setup() {
  #ifdef BAUD
  Serial.begin(BAUD);
  #else
  Serial.begin();
  #endif
  delay(2000);

  Serial.print("\nAttempting to connect to the Wi-Fi network with ");
  Serial.println(TITLE);

  #if defined(ARDUINO_XIAO_ESP32C6)
    #if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 0, 4)) 
      // reproduce initVariant() from ESP32 v3.0.4
      uint8_t WIFI_ENABLE = 3;
      uint8_t WIFI_ANT_CONFIG = 14;
      // enable the RF switch
      pinMode(WIFI_ENABLE, OUTPUT);
      digitalWrite(WIFI_ENABLE, LOW);
      // select the internal antenna
      pinMode(WIFI_ANT_CONFIG, OUTPUT);
      digitalWrite(WIFI_ANT_CONFIG, LOW);
    #endif

    // same code for ESP32 v3.0.2 and up
    #if defined(USE_EXTERNAL_ANTENNA)
      digitalWrite(WIFI_ANT_CONFIG, HIGH);
    #endif

    Serial.print("Using ");
    #ifdef USE_EXTERNAL_ANTENNA
      Serial.print("an external");
    #else
      Serial.print("the internal");
    #endif
    Serial.println(" antenna.");
  #endif

  WiFi.mode(WIFI_STA);
  #ifdef NEED_TX_POWER
    delay(25);
    if (WiFi.getTxPower() != TX_POWER) {
      WiFi.setTxPower(TX_POWER);
      delay(25);
    }
    Serial.printf("Wi-Fi TX power set to: %d\n", WiFi.getTxPower());
    delay(25);
  #endif
  connect_time = millis();
  WiFi.begin(ssid, password);
  while (!WiFi.isConnected()) {
    // do nothing;
  }  
  etime = millis() - connect_time;
  while ((uint32_t) WiFi.localIP() == (uint32_t) 0) {
    // do nothing
  };
  itime = millis() - connect_time;

}

unsigned long report = 0;

void loop() {
  if (millis() - report > 25000) {
    Serial.print("\nWiFi is connected with IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Time to connect: %lu ms\n", etime);
    Serial.printf("Time to valid IP local address: %lu ms\n", itime);
    Serial.printf("Difference: %lu ms\n", itime - etime);
    report = millis();
  }  
}
