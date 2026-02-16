/*
 *  See Zigbee_On_Off_Light.ino for license and attribution.
 */

#include <Arduino.h>
#include "Zigbee.h"

#include "MACs.h"

#ifndef ZIGBEE_MODE_ED
  #error Zigbee end device mode is not selected in Tools->Zigbee mode
#endif

//////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  GPIO pin used to control LED. Define here to override the
///  automatic definition done below
///#define LED_PIN xx
///
///  Signal level (HIGH or LOW) needed to turn on the LED. 
///  Define here to override the automatic definition done below.
///#define LED_ON xxx 
///
///  GPIO pin used by a push button that is connected to GROUND.
///  Define here to overide the automatic use of the BOOT button.
///#define BOOT_PIN xxxx
///
///  Rate of USB to Serial chip if used on the development board.
///  This is ignored when the native USB peripheral of the 
///  ESP SoC is used.
///
#define SERIAL_BAUD 115200
///
///  Time in milliseconds to wait after Serial.begin() in 
///  the setup() function. If not defined, it will be set
///  to 5000 if running in the PlaformIO IDE to manually switch
///  to the serial monitor otherwise to 2000 if an native USB 
///  peripheral is used or 1000 if a USB-serial adpater is used.
///*define SERIAL_BEGIN_DELAY 8000
///
//////////////////////////////////


#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #error ESP32 Arduino core version 3.3.6 or newer needed
#endif 

#if !defined(CONFIG_SOC_IEEE802154_SUPPORTED)
  #error The SoC must support IEEE 802.15.4 (Zigbee)
#endif

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  #define TITLE "Seeed XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA 
    #define ANTENNA "EXTERNAL"
  #else
    #define ANTENNA "ONBOARD CERAMIC"
  #endif
#elif defined(ARDUINO_BOARD)
  #define TITLE ARDUINO_BOARD
#else
  #define TITLE "Unknown ESP32 board"
#endif        


// Use correct builtin LED
#if defined(RGB_BUILTIN)       // not defined in XIAO_ESP32Cx pins_arduino.h
  #if !defined(LED_PIN)
    #define LED_PIN RGB_BUILTIN
  #endif
  #if !defined(LED_ON)
    #define LED_ON HIGH          // default for other boards in original code
  #endif
#elif defined(LED_BUILTIN)
  #if !defined(LED_PIN)
    #define LED_PIN LED_BUILTIN
  #endif
  #if !defined(LED_LOW)
    #define LED_ON LOW          // because I/O pin must be grounded to turn on LED
  #endif
#endif


//---- sanity checks -----------------

#if !defined(LED_PIN)
  #error LED_PIN not defined
#endif

#if !defined(LED_ON)
  #error LED_ON not defined
#endif

#if !defined(BOOT_PIN)
  #error BOOT_PIN not defined
#endif

#if !(ARDUINO_USB_CDC_ON_BOOT > 0) && !defined(SERIAL_BAUD)
  #error SERIAL_BAUD not defined
#endif

//</Configuration> ---------------------

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

  Serial.println("\n\n     Project: Zigbee On/Off Light");
  Serial.printf("       Board: %s\n", TITLE);
  #ifdef ANTENNA
  Serial.printf("     Antenna: %s\n", ANTENNA);
  #endif
  Serial.printf("IEEE Address: %s\n\n", ZIGBEE_MAC_STR);

  // Init LED and turn it OFF (if LED_PIN == RGB_BUILTIN, the rgbLedWrite() will be used under the hood)
  pinMode(led, OUTPUT);
  //digitalWrite(led, LOW);
  setLED(LOW);

  // Init button for factory reset
  pinMode(button, INPUT_PULLUP);


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
  int attempts = 0;
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    if (attempts > 64) {
      Serial.println("\nHaving difficulty connecting. May need to erase the flash");
      Serial.println("memory before uploading the firmware and may need to reboot");
      Serial.println("the Zigbee On_Off_Switch");
      attempts = 0;
    } else {
      attempts++;
    }
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nConnected.");
  Serial.println("Toggle the onboard LED with short boot button presses.");
  Serial.println("Perform a Zigbee factory reset and SoC reset with a");
  Serial.println("long boot button press that is longer than 3 seconds.");
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
