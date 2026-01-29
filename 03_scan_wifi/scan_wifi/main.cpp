/*
 *  See scan_wifi.ino for license and attribution.
 */

#include <Arduino.h>
#include "WiFi.h"

#include "MACs.h"

/////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  Rate of USB to Serial chip if used on the development board.
///  This is ignored when the native USB peripheral of the 
///  ESP SoC is used.
#define SERIAL_BAUD 115200
///
///  Time in milliseconds to wait after Serial.begin() in 
///  the setup() function. If not defined, it will be set
///  to 5000 if running in the PlaformIO IDE to manually switch
///  to the serial monitor otherwise to 2000 if an native USB 
///  peripheral is used or 1000 if a USB-serial adpater is used.
#define SERIAL_BEGIN_DELAY 10000
///
//////////////////////////////////

#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 5))    
  #error ESP32 Arduino core version 3.3.5 or newer needed
#endif

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "XIAO_ESP32C3"
  #define ANTENNA "V1.2 FPC"
#elif defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "XIAO_ESP32C5"
  #define ANTENNA "A-01 FPC"
#elif defined(ARDUINO_XIAO_ESP32C6)
  #define TITLE "XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA 
    #define ANTENNA "EXTERNAL"
  #else
    #define ANTENNA "ONBOARD CERAMIC"
  #endif
#elif defined(ARDUINO_XIAO_ESP32S3)
  #define TITLE "XIAO_ESP32S3"
  #define ANTENNA "V1.2 FPC"
#else
  #define TITLE "Unknown ESP32 board"
  #define ANTENNA "Unknown"
#endif  

void setup() {
  #if !defined(SERIAL_BEGIN_DELAY)
    #if defined(PLATFORMIO)
      #define SERIAL_BEGIN_DELAY 5000    // 5 seconds
    #elif (ARDUINO_USB_CDC_ON_BOOT > 0)
      #define SERIAL_BEGIN_DELAY 2000    // 2 seconds
    #else
      #define SERIAL_BEGIN_DELAY 1000    // 1 second
    #endif
  #endif 

  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
  Serial.println();
  #endif  

  #if defined(USE_EXTERNAL_ANTENNA) && defined(ARDUINO_XIAO_ESP32C6)
    //pinMode(WIFI_ANT_CONFIG, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\nProject: Wi-Fi Scan");
  Serial.printf("  Board: %s\n", TITLE);
  Serial.printf("STA MAC: %s\n", STA_MAC_STR);
  Serial.printf("Antenna: %s\n", ANTENNA);

  #if defined(CONFIG_SOC_WIFI_SUPPORT_5G) // currently only with the ESP32-C5
    Serial.println("\nThis ESP32 SoC supports the 2.4 and 5 GHz Wi-Fi bands.");
    Serial.println("Both bands will be scanned together next.");
    #if defined(ARDUINO_XIAO_ESP32C5)
      Serial.println("Try the 01_wifi_scan project @ https://github.com/sigmdel/xiao_esp32c5_sketches/");
    #else
      Serial.println("Try running the latest WiFiScan.ino example sketch in the arduino-esp32 WiFi library");
    #endif  
    Serial.println("to scan the bands together and separately.\n");
  #endif
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
    Serial.println("Nr | SSID                             |       BSSID       | RSSI |  CH | Encryption");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d | ", i + 1);
      Serial.printf("%-32.32s | ", WiFi.SSID(i).c_str());
      Serial.printf("%-17.17s | ", WiFi.BSSIDstr(i).c_str());
      Serial.printf("%4ld | ", WiFi.RSSI(i));
      Serial.printf("%3ld | ", WiFi.channel(i));
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
