/*
 *  See wifi_connect_time.ino for license and attribution.
 */

#include <Arduino.h>
#include "WiFi.h"

#include "MACs.h"
#include "secrets.h"

/////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  Define timeout in milliseconds while waiting to connect to the Wi-Fi network
///#define CONNECT_TIMEOUT 10000
///
///  Define timeout in millisecond while waiting for IP address from DHCP server
///#define DHCP_TIMEOUT 20000
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

#ifdef CONNECT_TIMEOUT
unsigned long connectTimeout = CONNECT_TIMEOUT;
#else
unsigned long connectTimeout = 10000;
#endif

#ifdef DHCP_TIMEOUT
unsigned long dhcpTimeout = DHCP_TIMEOUT;
#else
unsigned long dhcpTimeout = 20000;
#endif  

unsigned long ctimer = 0;        // start time of test
unsigned long connectTime = 0;   // time when connected, later time to connect after subtracting ctimer
unsigned long ipTime = 0;        // time to acquire IP address after connected
int arssi = 0;                   // absolute value of rssi 
int tcount = 0;

void disconnect(boolean quiet = true) {
  if (WiFi.isConnected()) {
    if (!quiet)
      Serial.print("Disconnecting...");
    WiFi.setAutoReconnect(false);
    WiFi.disconnect(true, true);
    while (WiFi.isConnected()) delay(5);
    if (!quiet) 
      Serial.println(" disconnected.");
  }
}

boolean runTest(void) {
  tcount++;
  Serial.printf("\n\nTest #%d\n", tcount);
  
  WiFi.mode(WIFI_STA);    
  ctimer = millis();
  WiFi.begin(ssid, password);
  while (!WiFi.STA.connected()) {
    if (millis() - ctimer > connectTimeout) {
      Serial.printf("Test aborted, could not connect within %lu ms\n", connectTimeout);
      connectTime = 0;
      ipTime = 0;
      return false;
    }  
    // else do nothing;
  }  
  connectTime = millis();  
  while ((uint32_t) WiFi.localIP() == (uint32_t) 0) {
    if (millis() - connectTime > dhcpTimeout) {
      Serial.printf("Test aborted, could not get IP address within %lu ms\n", dhcpTimeout);
      return false;
    }  
    // else do nothing
  };
  ipTime = millis() - connectTime;
  connectTime = connectTime - ctimer;
  arssi = abs(WiFi.RSSI());
  return true;
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

  Serial.println("\n\n Project: wifi_connect_time");
  Serial.println(" Purpose: Measure Wi-Fi connection times and signal strength");
  Serial.printf("   Board: %s\n", TITLE);
  Serial.printf(" STA MAC: %s\n", STA_MAC_STR);
  Serial.printf(" Antenna: %s\n", ANTENNA);
  Serial.printf(" Network: %s\n", ssid);

  WiFi.mode(WIFI_STA);
  #ifdef TX_POWER
    Serial.printf("\n Setting Wi-Fi TX power to %d\n", TX_POWER);
    delay(25);
    if (WiFi.getTxPower() != TX_POWER) {
      WiFi.setTxPower(TX_POWER);
      delay(25);
    }
    Serial.printf(" Wi-Fi TX power is now: %d\n", WiFi.getTxPower());
  #else
    // Default Wi-Fi TX Poweer Level
    Serial.printf("TX power: %d\n", WiFi.getTxPower());
  #endif
  
  disconnect();
  delay(1000);
}

unsigned long min_connectTime = 2000000000;
unsigned long max_connectTime = 0;
unsigned long connectTime_sum = 0;

unsigned long min_ipTime = 2000000000;
unsigned long max_ipTime = 0;
unsigned long ipTime_sum = 0;

int min_arssi = 2000000000;
int max_arssi = 0;
long arssi_sum = 0;

int valid_tests = 0;
int failed_tests = 0;

void loop() {
  if (runTest()) {
    valid_tests++;
    Serial.print("Board connected with IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Time to connect to network: %lu ms\n", connectTime);
    Serial.printf("Time to acquire IP address: %lu ms\n", ipTime);
    Serial.printf("RSSI: -%d dBm\n", arssi);

    if (connectTime < min_connectTime) 
      min_connectTime = connectTime;
    if (connectTime > max_connectTime) 
      max_connectTime = connectTime;
    connectTime_sum += connectTime;

    if (ipTime < min_ipTime) 
      min_ipTime = ipTime;
    if (ipTime > max_ipTime) 
      max_ipTime = ipTime;
    ipTime_sum += ipTime;

    if (arssi < min_arssi) 
      min_arssi = arssi;
    if (arssi > max_arssi)
      max_arssi = arssi;
    arssi_sum += arssi;

    if (!(valid_tests % 5)) {
      Serial.println();
      Serial.printf("Summary statistics after %d valid tests\n", valid_tests);
      if (failed_tests) {
        printf("Average times do not take into consideration %d failed tests (%.1f %%)\n", failed_tests, (100*failed_tests)/(1.0*(failed_tests+valid_tests)));
      }  
      Serial.println();
      Serial.printf("Minimum time to connect: %lu ms\n", min_connectTime);
      Serial.printf("Maximum time to connect: %lu ms\n", max_connectTime);
      Serial.printf("Average time to connect: %lu ms\n", connectTime_sum/valid_tests);

      Serial.printf("\nMinimum time to acquire IP address: %lu ms\n", min_ipTime);
      Serial.printf("Maximum time to acquire IP address: %lu ms\n", max_ipTime);
      Serial.printf("Average time to acquire IP address: %lu ms\n", ipTime_sum/valid_tests);
    
      Serial.printf("\nMinimum RSSI: -%d dBm\n", max_arssi);
      Serial.printf("Maximum RSSI: -%d dBm\n", min_arssi);
      Serial.printf("Average RSSI: -%ld dBm\n", arssi_sum/valid_tests);
      Serial.println();
    }   
  } else {
    failed_tests++;
    Serial.printf("\n\n%d failed tests in %d tests\n\n", failed_tests, failed_tests + valid_tests);
  }  
  disconnect();
  delay(10000); // delay 10 seconds for next test, some one else may want to use the network
}
