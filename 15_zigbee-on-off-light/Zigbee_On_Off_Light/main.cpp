// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// See Zigbee_On_Off_Light.ino for license 
// 
// Source: 
// https://github.com/espressif/arduino-esp32/blob/3.1.1/libraries/Zigbee/examples/Zigbee_On_Off_Light/Zigbee_On_Off_Light.ino

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

///<Configuration> ---------------------

// Use correct builtin LED
#if defined(RGB_BUILTIN)       // not defined in XIAO_ESP32C6 variant pins_arduino.h
  #define LED_PIN RGB_BUILTIN
  #define LED_ON HIGH          // default for other boards in original code
  #define SERIAL_BAUD 115200
#elif defined(LED_BUILTIN)
  #define LED_PIN LED_BUILTIN
  #define LED_ON LOW           // because I/O pin must be grounded to turn on LED
  #define SERIAL_BAUD
#else
  #error "LED pin not defined"
#endif

#if defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  // Uncomment the following macro to use a connected external antenna.
  //#define USE_EXTERNAL_ANTENNA
#else
  #undef USE_EXTERNAL_ANTENNA
#endif    

// Define this print to the console a test number at startup
#define TEST_NO 2

///</Configuration> ---------------------

uint8_t led = LED_PIN;
uint8_t button = BOOT_PIN;

/* Zigbee light bulb configuration */
#define ZIGBEE_LIGHT_ENDPOINT 10

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);

/********************* RGB LED functions **************************/
void setLED(bool value) { 
  //(if LED_PIN == RGB_BUILTIN, then rgbLedWrite() will be used under the hood)
  digitalWrite(LED_PIN, (value) ? LED_ON : 1 - LED_ON);
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(5000);
  #ifdef TEST_NO
  Serial.printf("Zigbee_On_Off_Light, test #%d\n\n", TEST_NO); // 
  #endif 

  // Init LED and turn it OFF (if LED_PIN == RGB_BUILTIN, the rgbLedWrite() will be used under the hood)
  pinMode(led, OUTPUT);
  //digitalWrite(led, LOW);
  setLED(LOW);

  // Init button for factory reset
  pinMode(button, INPUT_PULLUP);

  #if defined(ARDUINO_XIAO_ESP32C6) && defined(USE_EXTERNAL_ANTENNA)
    // Assuming ESP_ARDUINO_VERSION >= 3.0.4
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  //Optional: set Zigbee device name and model
  zbLight.setManufacturerAndModel("Espressif", "ZBLightBulb");

  // Set callback function for light change
  zbLight.onLightChange(setLED);

  //Add endpoint to Zigbee Core
  Serial.println("Adding ZigbeeLight endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbLight);

  // When all EPs are registered, start Zigbee. By default acts as ZIGBEE_END_DEVICE
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
}

void loop() {
  // Checking button for factory reset
  if (digitalRead(button) == LOW) {  // Boot push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If button pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    // Toggle the on board LED when if button is released in less than 3 seconds
    zbLight.setLight(!zbLight.getLightState());
  }
  delay(100);
}
