/*
 *  See MatterOnOffLight.ino for license and attribution.
 */

#include <Arduino.h>
#include <Matter.h>
#include <Preferences.h>

#include "MACs.h"

    // CONFIG_ENABLE_CHIPOBLE is enabled when BLE is used to commission the Matter Network
#if !CONFIG_ENABLE_CHIPOBLE
// if the device can be commissioned using BLE, WiFi is not used - save flash space
#include <WiFi.h> 
#include "secrets.h"
#endif


/////// User configuration //////
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
///#define SERIAL_BEGIN_DELAY 8000
///
//////////////////////////////////


#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #error ESP32 Arduino core version 3.3.6 or newer needed
#endif 

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  #define TITLE "Seeed XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA 
    #define ANTENNA "External"
  #else
    #define ANTENNA "Onboard ceramic"
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
  #if !defined(LED_ON)
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

const uint8_t ledPin = LED_PIN;
const uint8_t buttonPin = BOOT_PIN; 

// List of Matter Endpoints for this Node
// On/Off Light Endpoint
MatterOnOffLight OnOffLight;

// it will keep last OnOff state stored, using Preferences
Preferences matterPref;
const char *onOffPrefKey = "OnOff";


// Button control
uint32_t button_time_stamp = 0;                // debouncing control
bool button_state = false;                     // false = released | true = pressed
const uint32_t debouceTime = 250;              // button debouncing time (ms)
const uint32_t decommissioningTimeout = 5000;  // keep the button pressed for 5s, or longer, to decommission

// Matter Protocol Endpoint Callback
bool setLightOnOff(bool state) {
  Serial.printf("User Callback :: New Light State = %s\r\n", state ? "ON" : "OFF");
  digitalWrite(ledPin, (state) ? LED_ON : 1 - LED_ON);
  // store last OnOff state for when the Light is restarted / power goes off
  matterPref.putBool(onOffPrefKey, state);
  // This callback must return the success state to Matter core
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
    //pinMode(WIFI_ANT_CONFIG, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\nProject: MatterOnOffLight");
  Serial.printf("        Board: %s\n", TITLE);
  #ifdef ANTENNA
  Serial.printf("      Antenna: %s\n", ANTENNA);
  #endif
  Serial.printf("Wi-Fi STA MAC: %s\n", STA_MAC_STR);
  Serial.printf("Bluetooth MAC: %s\n\n", BT_MAC_STR);
 
  // Initialize the USER BUTTON (Boot button) GPIO that will act as a toggle switch
  pinMode(buttonPin, INPUT_PULLUP);
  // Initialize the LED (light) GPIO and Matter End Point
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1-LED_ON);


// CONFIG_ENABLE_CHIPOBLE is enabled when BLE is used to commission the Matter Network
#if !CONFIG_ENABLE_CHIPOBLE
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Manually connect to WiFi
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\r\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);
#endif

  // Initialize Matter EndPoint
  matterPref.begin("MatterPrefs", false);
  bool lastOnOffState = matterPref.getBool(onOffPrefKey, true);
  OnOffLight.begin(lastOnOffState);
  OnOffLight.onChange(setLightOnOff);

  // Matter beginning - Last step, after all EndPoints are initialized
  Matter.begin();
  // This may be a restart of a already commissioned Matter accessory
  if (Matter.isDeviceCommissioned()) {
    Serial.println("Matter Node is commissioned and connected to the network. Ready for use.");
    Serial.printf("Initial state: %s\r\n", OnOffLight.getOnOff() ? "ON" : "OFF");
    OnOffLight.updateAccessory();  // configure the Light based on initial state
  } else {
    // https://docs.espressif.com/projects/arduino-esp32/en/latest/matter/matter.html
    Serial.printf("isWiFiConnected: %s\n", (Matter.isWiFiConnected()) ? "true" : "false");
    Serial.printf("isThreadConnected: %s\n", (Matter.isThreadConnected()) ? "true" : "false");
    Serial.printf("isDeviceConnected: %s\n", (Matter.isDeviceConnected()) ? "true" : "false");
    Serial.printf("isBLECommissioningEnabled(): %s\n", (Matter.isBLECommissioningEnabled()) ? "true" : "false");
  }
}


void loop() {
  // Check Matter Light Commissioning state, which may change during execution of loop()
  if (!Matter.isDeviceCommissioned()) {
    Serial.println("");
    Serial.println("Matter Node is not commissioned yet.");
    Serial.println("Initiate the device discovery in your Matter environment.");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\r\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\r\n", Matter.getOnboardingQRCodeUrl().c_str());
    // waits for Matter Light Commissioning.
    uint32_t timeCount = 0;
    while (!Matter.isDeviceCommissioned()) {
      delay(100);
      if ((timeCount++ % 50) == 0) {  // 50*100ms = 5 sec
        Serial.println("Matter Node not commissioned yet. Waiting for commissioning.");
      }
    }
    Serial.printf("Initial state: %s\r\n", OnOffLight.getOnOff() ? "ON" : "OFF");
    OnOffLight.updateAccessory();  // configure the Light based on initial state
    Serial.println("Matter Node is commissioned and connected to the network. Ready for use.");
  }

  // A button is also used to control the light
  // Check if the button has been pressed
  if (digitalRead(buttonPin) == LOW && !button_state) {
    // deals with button debouncing
    button_time_stamp = millis();  // record the time while the button is pressed.
    button_state = true;           // pressed.
  }

  // Onboard User Button is used as a Light toggle switch or to decommission it
  uint32_t time_diff = millis() - button_time_stamp;
  if (button_state && time_diff > debouceTime && digitalRead(buttonPin) == HIGH) {
    button_state = false;  // released
    // Toggle button is released - toggle the light
    Serial.println("User button released. Toggling Light!");
    OnOffLight.toggle();  // Matter Controller also can see the change
  }

  // Onboard User Button is kept pressed for longer than 5 seconds in order to decommission matter node
  if (button_state && time_diff > decommissioningTimeout) {
    Serial.println("Decommissioning the Light Matter Accessory. It shall be commissioned again.");
    OnOffLight.setOnOff(false);  // turn the light off
    Matter.decommission();
    button_time_stamp = millis();  // avoid running decommissining again, reboot takes a second or so
  }
}
