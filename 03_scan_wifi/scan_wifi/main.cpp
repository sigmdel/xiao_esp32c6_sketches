/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 *  Source: https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi/examples/WiFiScan
 *  This appears to be released under the GNU LESSER GENERAL PUBLIC LICENSE 
 *    see: https://github.com/espressif/arduino-esp32/blob/master/LICENSE.md
 */
#include <Arduino.h>
#include "WiFi.h"

//#define ENABLE_RF_SWITCH  
//#define USE_EXTERNAL_ANTENNA

void setup() {
  Serial.begin();
  // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);

  // Take care of RF switch according to the two macros defined above and 
  // the version of the ESP32 Arduino core being used.
  if (ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 4)) {
    // In ESP32 3.0.4+ the initVarian() function enables the RF switch 
    // and selects the internal antenna
    #ifndef ENABLE_RF_SWITCH
    digitalWrite(WIFI_ENABLE, HIGH); // disable the switch
    #endif
    #ifdef USE_EXTERNAL_ANTENNA
    digitalWrite(WIFI_ANT_CONFIG, HIGH); // select the external antenna
    #endif
   } else {
    // no initVariant() in older ESP32 cores
    #ifdef ENABLE_RF_SWITCH
    pinMode(3, OUTPUT);
    digitalWrite(3, LOW);
    #endif
    #ifdef USE_EXTERNAL_ANTENNA
    pinMode(14, OUTPUT);
    digitalWrite(14, HIGH);
    #endif
  }

  Serial.print("The RF switch is ");
  #ifndef ENABLE_RF_SWITCH  
  Serial.println("not ");
  #endif
  Serial.println("enabled.");

  Serial.print("Using ");
  #ifdef USE_EXTERNAL_ANTENNA
    Serial.print("an external");
  #else
    Serial.print("the internal");
  #endif
  Serial.println(" antenna.\nSetup done");
}

void loop() {
  Serial.println("\nScan start");

  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      Serial.printf("%4ld", WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2ld", WiFi.channel(i));
      Serial.print(" | ");
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:            Serial.print("open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
        default:                        Serial.print("unknown");
      }
      Serial.println();
      delay(10);
    }
  }
  Serial.println("");

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  // Wait a bit before scanning again.
  delay(5000);
}
