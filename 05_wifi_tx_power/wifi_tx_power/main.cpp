/*
 *  See wifi_tx_power.ino for license and attribution.
 */

#include <Arduino.h>
#include <WiFi.h>

#include "MACs.h"
#include "secrets.h"



////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  Define timeout in milliseconds while waiting to connect to the Wi-Fi network
///#define TIMEOUT 120000
///
///  If it is necessary to adjust the WIFI TX_POWER (super mini esp32c3) specify the 
///  wifi_power_t value. Find all possible values here 
///  https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFiGeneric.h
///#define TX_POWER WIFI_POWER_11dBm
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
///#define SERIAL_BEGIN_DELAY 10000
///
//////////////////////////////////

#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #error ESP32 Arduino core version 3.3.6 or newer needed
#endif

//---- Identify the ESP32 board and antenna ----

#if defined(NOLOGO_ESP32C3_SUPER_MINI)
  #define TITLE "NOLOGO_ESP32C3_SUPER_MINI"
  #define ANTENNA "ONBOARD CERAMIC"
  #warning Some ESP32C3 Super Mini models will only connect with an adjustment of the TX Power level, if at all
#elif defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "XIAO_ESP32C3"
  #define ANTENNA "V1.2 FPC"
  //#elif defined(ARDUINO_XIAO_ESP32C5)  // not yet supported
  //  #define TITLE "XIAO_ESP32C5"
  //  #define ANTENNA "A-01 FPC"
#elif defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  #define TITLE "XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA 
    #define ANTENNA "EXTERNAL"
    #warning Connect the external antenna before running this sketch
  #else
    #define ANTENNA "ONBOARD CERAMIC"
  #endif
#elif defined(ARDUINO_XIAO_ESP32S3)
  #define TITLE "XIAO_ESP32S3"
  #define ANTENNA "V1.2 FPC"
#elif defined(ARDUINO_ESP32S3_DEV)
  #if defined(WROOM_DUAL_USB)
    #define TITLE "ESP32S3_WROOM"
    #define ANTENNA "PCB TRACE"
  #elif defined(SUPER_MINI_S3)
    #define TITLE "SUPER_MINI_S3"
    #define ANTENNA "INTERNAL CERAMIC"
    #warning May have to manually enable bootloader mode to upload the firmware and, after, to manually reset the board to return to normal operating mode
    #warning in ArduinoIDE - select "ESP32S3 Dev" Module , then in Tools  USB CDC on Boot: "Enabled", PSRAM: "QSPI PSRAM"
  #else 
    #define TITLE "Unknown ESP32-S3 board"    
    #define ANTENNA "Unknown"
  #endif  
#elif defined(ARDUINO_LOLIN_S2_MINI) 
  #define TITLE "LOLIN_S2_MINI"   
  #define ANTENNA  "PCB TRACE"
#elif defined(ARDUINO_LOLIN32_LITE)
  #define TITLE "LOLIN32_LITE"   
  #define ANTENNA "PCB TRACE"
#elif defined(ARDUINO_LILYGO_T_DISPLAY)  
  #define TITLE "LILYGO_T_DISPLAY"
  #define ANTENNA "PCP TRACE"
#else
  #define TITLE "Unknown ESP32 board"
  #define ANTENNA "Unknown"
#endif  

#ifndef TIMEOUT
#define TIMEOUT 120000
#endif

/*
 from ~/.arduino15/packages/esp32/hardware/esp32/3.0.1/libraries/WiFi/src/WiFiGeneric.h
 from ~/.platformio/packages/framework-arduinoespressif32/libraries/WiFi/src/WiFiGeneric.h

typedef enum {
    WIFI_POWER_19_5dBm = 78,// 19.5dBm
    WIFI_POWER_19dBm = 76,// 19dBm
    WIFI_POWER_18_5dBm = 74,// 18.5dBm
    WIFI_POWER_17dBm = 68,// 17dBm
    WIFI_POWER_15dBm = 60,// 15dBm
    WIFI_POWER_13dBm = 52,// 13dBm
    WIFI_POWER_11dBm = 44,// 11dBm
    WIFI_POWER_8_5dBm = 34,// 8.5dBm
    WIFI_POWER_7dBm = 28,// 7dBm
    WIFI_POWER_5dBm = 20,// 5dBm
    WIFI_POWER_2dBm = 8,// 2dBm
    WIFI_POWER_MINUS_1dBm = -4// -1dBm
} wifi_power_t;
*/

const int powerCount = 13;

int power[powerCount] = {
  0,
  WIFI_POWER_19_5dBm, WIFI_POWER_19dBm, WIFI_POWER_18_5dBm, WIFI_POWER_17dBm,
  WIFI_POWER_15dBm,   WIFI_POWER_13dBm, WIFI_POWER_11dBm,   WIFI_POWER_8_5dBm,
  WIFI_POWER_7dBm,    WIFI_POWER_5dBm,  WIFI_POWER_2dBm,    WIFI_POWER_MINUS_1dBm};

const char *powerstr[powerCount] = {
  "WIFI_POWER_default",
  "WIFI_POWER_19_5dBm", "WIFI_POWER_19dBm", "WIFI_POWER_18_5dBm", "WIFI_POWER_17dBm",
  "WIFI_POWER_15dBm",   "WIFI_POWER_13dBm", "WIFI_POWER_11dBm",   "WIFI_POWER_8_5dBm",
  "WIFI_POWER_7dBm",    "WIFI_POWER_5dBm",  "WIFI_POWER_2dBm",    "WIFI_POWER_MINUS_1dBm"};

int truepower[powerCount] = {0};

long ctimes[powerCount] = {0};

int rssi[powerCount] = {0};

void print_ctimes(void) {
  bool err = false;
  Serial.println("Connect time vs txPower\n");
  Serial.printf("\nBoard: %s\n", TITLE);
  Serial.print("Using ");
  #ifdef USE_EXTERNAL_ANTENNA
  Serial.print("an external");
  #else
  Serial.print("the internal");
  #endif
  Serial.println(" antenna.");

  Serial.printf("%24s | %5s | %5s | %5s | %8s\n", "enum", "want", "set", "rssi", "time (ms)");
  Serial.printf("%26s %7s %7s %7s\n", "|", "|", "|", "|");
  for (int i=0; i<powerCount; i++) {
     Serial.printf("%24s | %5d | %5d | %5d | %8lu", powerstr[i], power[i], truepower[i], rssi[i], ctimes[i]);
     if (truepower[i] != power[i] ) {
      Serial.print(" *");
      err = true;
     }
     Serial.println();
  }
  if (err)
    Serial.println("  * = unable to set transmit power to desired value");
}

unsigned long connect_time = 0;
unsigned long etime = 0;
unsigned long dottime = 0;

void start_connecting(int pwindex = 0) {
  if ((pwindex < 0) || (pwindex >= powerCount)) return;
  
  Serial.printf("\nTest #%d/%d\n", pwindex, powerCount-1);
  if (WiFi.isConnected()) {
    Serial.print("Disconnecting...");
    WiFi.setAutoReconnect(false);
    WiFi.disconnect(true, true);
    while (WiFi.isConnected()) delay(5);
    Serial.println(" disconnected. Wait 1 second.");
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  delay(25);
  int cpower = power[pwindex];
  WiFi.setTxPower((wifi_power_t)cpower);
  delay(25);
  Serial.printf("Setting txPower to %s (%d)\n", powerstr[pwindex], cpower);
  int tpower = WiFi.getTxPower();
  if (tpower != cpower) Serial.println("Unable to set txPower");
  truepower[pwindex] = tpower;
  Serial.printf("Running pwindex %d, wanted txPower %s (%d), set to %d\n", pwindex, powerstr[pwindex], cpower, tpower);
  Serial.print("Connecting to Wi-Fi network... ");
 
  connect_time = millis();
  dottime = millis();
  WiFi.begin(ssid, password);
}

void getStatus(void) {
  switch(WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("[WiFi] SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
        return;
        break;
      case WL_CONNECTION_LOST:
        Serial.println("[WiFi] Connection was lost");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("[WiFi] Scan is completed");
        break;
      case WL_DISCONNECTED:
        Serial.println("[WiFi] WiFi is disconnected");
        break;
      case WL_CONNECTED:
        Serial.print("[WiFi] WiFi is connected. IP address: ");
        Serial.println(WiFi.localIP());
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
  }
}


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
    //pinMode(WIFI_ANT_CONFIG, OUTPUT); //done in .../variants/XIAO_ESP32C6/variant.cpp
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\nProject: wifi_tx_power");
  Serial.println("Purpose: Measure time to connect to a WiFi network as a function of WiFi TX power");
  Serial.printf("  Board: %s\n", TITLE);
  Serial.printf("STA MAC: %s\n", STA_MAC_STR);
  Serial.printf("Antenna: %s\n", ANTENNA);
  Serial.printf("Network: %s\n", ssid);

  WiFi.mode(WIFI_STA);
  delay(25);
  power[0] = WiFi.getTxPower();  // default tx power on boot
  start_connecting(0);
}

int txIndex = 0;

void loop() {
  if (txIndex == powerCount)  {
    print_ctimes();
    txIndex++;
  }
  if (txIndex > powerCount) return;

  if (millis() - dottime > 2000) {
    Serial.print('.');
    dottime = millis();
  }

  if (WiFi.isConnected()) {
    etime = millis() - connect_time;
    delay(1500); // see wifi_blackhole
    Serial.print(" connected with IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Time to connect: %lu ms\n", etime);
    ctimes[txIndex] = etime;
    rssi[txIndex] = WiFi.RSSI();
    Serial.printf("RSSI: %d dBm\n", rssi[txIndex]);   
    delay(1000);
    txIndex++;
    if (txIndex >= powerCount) return;
    start_connecting(txIndex);
  }

  if (millis() - connect_time > TIMEOUT) {
    Serial.printf("\nNot connected after %d ms\n", TIMEOUT);
    getStatus();
    ctimes[txIndex] = 2*TIMEOUT;
    txIndex++;
    if (txIndex >= powerCount) return;
    Serial.println("Retrying in 5 seconds with different txPower");
    delay(5000);
    start_connecting(txIndex);
  }
}
