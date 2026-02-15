/*
 *  See xiao32c6_antenna.ino for license and attribution.
 */

/*
 * It is not recommended to use the Wi-Fi capabilities of an ESP32 without a connected
 * antenna. However, this sketch does not use the radio to transmit any signals and 
 * it does not appear that any damage has been caused when scanning using the RF switch
 * when set to use the external antenna without one being connected. Nevertheless, no 
 * guarantee is offered on that count, or any other for that matter.
*/ 

#include <Arduino.h>
#include "WiFi.h"

#include "MACs.h"

#ifndef ARDUINO_XIAO_ESP32C6
#error Must be using a XIAO ESP32C6 board only
#endif

#define INTERNAL_ANTENNA LOW
#define EXTERNAL_ANTENNA HIGH

#define ENABLE_RF_SWITCH LOW
#define DISABLE_RF_SWITCH HIGH

/* 
 These are defined as 
    static const uint8_t WIFI_ENABLE = 3;
    static const uint8_t WIFI_ANT_CONFIG = 14;         
 in ESP32 3.0.4 
*/ 
static const uint8_t RF_SWITCH_VDD = 3;
static const uint8_t RF_SWITCH_VCTL = 14;

/*
 Each IO pad can be in one of 3 states:
    a) mode INPUT
    b) mode OUTPUT value 0
    c) mode OUTPUT value 1
 so that there are 9 possible combinations or
 tests to run.
*/ 
void setTest(int test) {
  Serial.printf("Test #%d\n", test);
  switch (test) {

  case 1:
    pinMode(RF_SWITCH_VDD, INPUT);
    pinMode(RF_SWITCH_VCTL, INPUT);
    Serial.println("RF_SWITCH_VDD and RF_SWITCH_VCTL I/O pins are in INPUT mode; the RF switch is in its default state");
    break;
  case 2:
    pinMode(RF_SWITCH_VDD, INPUT);
    pinMode(RF_SWITCH_VCTL, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VCTL, INTERNAL_ANTENNA);
    Serial.println("RF_SWITCH_VDD I/O pin in INPUT mode; RF_SWITCH_VCTL is OUTPUT = INTERNAL_ANTENNA (LOW)");
    break;
  case 3:
    pinMode(RF_SWITCH_VDD, INPUT);
    pinMode(RF_SWITCH_VCTL, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VCTL, EXTERNAL_ANTENNA);
    Serial.println("RF_SWITCH_VDD I/O pin in INPUT mode; RF_SWITCH_VCTL is OUTPUT = EXTERNAL_ANTENNA (HIGH)");
    break;

  case 4:
    pinMode(RF_SWITCH_VDD, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VDD, DISABLE_RF_SWITCH);
    pinMode(RF_SWITCH_VCTL, INPUT);
    Serial.println("RF_SWITCH_VDD is OUTPUT = DISABLE_RF_SWITCH (HIGH); RF_SWITCH_VCTL I/O pin in INPUT mode");
    break;
  case 5:
    pinMode(RF_SWITCH_VDD, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VDD, DISABLE_RF_SWITCH);
    delay(100);
    pinMode(RF_SWITCH_VCTL, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VCTL, INTERNAL_ANTENNA);
    Serial.println("RF_SWITCH_VDD is OUTPUT = DISABLE_RF_SWITCH (HIGH); RF_SWITCH_VCTL is OUTPUT = INTERNAL_ANTENNA (LOW)");
    break;
  case 6:
    pinMode(RF_SWITCH_VDD, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VDD, DISABLE_RF_SWITCH);
    delay(100);
    pinMode(RF_SWITCH_VCTL, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VCTL, EXTERNAL_ANTENNA);
    Serial.println("RF_SWITCH_VDD is OUTPUT = DISABLE_RF_SWITCH (HIGH); RF_SWITCH_VCTL is OUTPUT = EXTERNAL_ANTENNA (HIGH)");
    break;

  case 7:
    pinMode(RF_SWITCH_VDD, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VDD, ENABLE_RF_SWITCH);
    pinMode(RF_SWITCH_VCTL, INPUT);
    Serial.println("RF_SWITCH_VDD is OUTPUT = ENABLE_RF_SWITCH (LOW); RF_SWITCH_VCTL I/O pin in INPUT mode");
    break;
  case 8:
    pinMode(RF_SWITCH_VDD, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VDD, ENABLE_RF_SWITCH);
    delay(100);
    pinMode(RF_SWITCH_VCTL, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VCTL, INTERNAL_ANTENNA);
    Serial.println("RF_SWITCH_VDD is OUTPUT = ENABLE_RF_SWITCH (LOW); RF_SWITCH_VCTL is OUTPUT = INTERNAL_ANTENNA (LOW)");
    break;
  case 9:
    pinMode(RF_SWITCH_VDD, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VDD, ENABLE_RF_SWITCH);
    delay(100);
    pinMode(RF_SWITCH_VCTL, OUTPUT);
    delay(100);
    digitalWrite(RF_SWITCH_VCTL, EXTERNAL_ANTENNA);
    Serial.println("RF_SWITCH_VDD is OUTPUT = ENABLE_RF_SWITCH (LOW); RF_SWITCH_VCTL is OUTPUT = EXTERNAL_ANTENNA (HIGH)");
    break;
  default:
    Serial.println("Invalid test number"); 
  }  
}

#define SCANS_PER_TEST 3

void doScan(int test) {
  if (test) 
    setTest(test); // skip test 0 done in setup()
  for  (int i=0; i<SCANS_PER_TEST; i++) {
    Serial.println("Scan start");
    // WiFi.scanNetworks will return the number of networks found.
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");
    if (n == 0) {
      Serial.println("no networks found");
    } else {
      Serial.print(n);
      Serial.println(" networks found");
                         //12345678901234567890123456789012     
      Serial.println("Nr | SSID                   | RSSI | CH |");
      int sum = 0;
      for (int i = 0; i < n; ++i) {
        // Print SSID and RSSI for each network found
        Serial.printf("%2d", i + 1);
        Serial.print(" | ");
        Serial.printf("%-22.22s", WiFi.SSID(i).c_str());
        Serial.print(" | ");
        int rssi = WiFi.RSSI(i);
        Serial.printf("%4ld", rssi);
        sum += rssi;
        Serial.print(" | ");
        Serial.printf("%2ld", WiFi.channel(i));
        Serial.println(" |");
        delay(10);
      }
      Serial.printf("              Average  RSSI |  %.1f\n", (float) sum/n);
    }
    Serial.println("");
    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();
    // Wait a bit before scanning again.
    delay(5000);
  } 
}
  

void setup() {
  Serial.begin();
  // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);


  // Time to start serial terminal (such as picocom) to capture test resulsts
  //   $ picocom --imap lfcrlf /dev/ttyACM0
  int startdelay = 8;
  for (int i=startdelay; i > 0; i--) {
    Serial.printf("Test will begin in %d seconds\n", i);
    delay(1000);
  }

  Serial.println("\n\nProject: xiao32c6_antenna");
  Serial.println("Purpose: Testing the antenna selection switch");
  Serial.println("  Board: XIAO ESP32C6");
  Serial.printf("STA MAC: %s\n", STA_MAC_STR);
  Serial.printf("ESP32 Arduino version %s\n", ESP_ARDUINO_VERSION_STR);


  // Start with a scan without modifying the IO pin 3 and 14 to confirm 
  // that initVariant() in ESP32 core 3.0.4+ modifies the use of the RF switch
  Serial.println("\nInitial scan with default antenna settings");
  Serial.println("--------------------------------------------");
  
  doScan(0);
}

void loop() {
  Serial.println("\nStarting test suite");
  Serial.println("-------------------");

  for (int test=1; test<10; test++) {
    doScan(test);
  } 
  Serial.println("\nTest suite completed.");
  Serial.println("Will restart in 5 minutes");
  delay(5*60*1000);
}
