// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// 
// Source: 
// https://github.com/espressif/arduino-esp32/blob/3.1.0-RC3/libraries/Zigbee/examples/Zigbee_On_Off_Light/Zigbee_On_Off_Light.ino

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include <Arduino.h>
#include "Zigbee.h"

#if defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  // Uncomment the following macro to use a connected external antenna.
  //#define USE_EXTERNAL_ANTENNA
#else
  #undef USE_EXTERNAL_ANTENNA
#endif    

#define BUTTON_PIN            9  // ESP32-C6/H2 Boot button
#define ZIGBEE_LIGHT_ENDPOINT 10

// Use correct builtin LED
#if defined(RGB_BUILTIN)
  #define LED_PIN RGB_BUILTIN
#elif defined(LED_BUILTIN)
  #define LED_PIN LED_BUILTIN
  #define LED_ON LOW           // LOW = I/O pin must be grounded to turn on LED
#else
  #error "NO LED defined"
#endif

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);

/********************* RGB LED functions **************************/
void setLED(bool value) {
#if defined(RGB_BUILTIN)
  digitalWrite(LED_PIN, value);  //(if LED_PIN == RGB_BUILTIN, the rgbLedWrite() will be used under the hood)
#else  
  digitalWrite(LED_PIN, (value) ? LED_ON : 1 - LED_ON);
#endif 
}

/********************* Arduino functions **************************/
void setup() {
  // Init LED and turn it OFF 
  pinMode(LED_PIN, OUTPUT);
  setLED(LOW);
 
  // Init button for factory reset
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  #if defined(ARDUINO_XIAO_ESP32C6) && defined(USE_EXTERNAL_ANTENNA)
    // Assuming ESP_ARDUINO_VERSION >= 3.0.4
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  //Optional: set Zigbee device name and model
  zbLight.setManufacturerAndModel("Espressif", "ZBLightBulb");

  // Set callback function for light change
  zbLight.onLightChange(setLED);

  //Add endpoint to Zigbee Core
  log_d("Adding ZigbeeLight endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbLight);

  // When all EPs are registered, start Zigbee. By default acts as ZIGBEE_END_DEVICE
  log_d("Calling Zigbee.begin()");
  Zigbee.begin();
}

void loop() {
  // Checking button for factory reset
  if (digitalRead(BUTTON_PIN) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.printf("Resetting Zigbee to factory settings, reboot.\n");
        Zigbee.factoryReset();
      }
    }
  }
  delay(100);
}
