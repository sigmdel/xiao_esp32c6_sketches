/*
 *  See MatterSmartButton.ino for license and attribution.
 */

#include <Arduino.h>
#include <Matter.h>

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
///  GPIO pin used by a push button that is connected to GROUND.
///  Define here to overide the automatic use of the BOOT button.
///#define BOOT_PIN xxxx
///
///  Push button debounce time in milliseconds (ms)
#define DEBOUNCE_TIME 250
///
///  Long button press time (ms) to start decommissioning the smart button
#define DECOMMISSIONING_TIMEOUT 5000
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


//---- sanity checks -----------------

#if !defined(BOOT_PIN)
  #error BOOT_PIN not defined
#endif

#if !(ARDUINO_USB_CDC_ON_BOOT > 0) && !defined(SERIAL_BAUD)
  #error SERIAL_BAUD not defined
#endif


// List of Matter Endpoints for this Node
// Generic Switch Endpoint - works as a smart button with a single click
MatterGenericSwitch SmartButton;


// set your board USER BUTTON pin here
const uint8_t buttonPin = BOOT_PIN;  // Set your pin here. Using BOOT Button.

// Button control
uint32_t button_time_stamp = 0;                // debouncing control
bool button_state = false;                     // false = released | true = pressed
const uint32_t debounceTime = DEBOUNCE_TIME;    // button debouncing time (ms)
const uint32_t decommissioningTimeout = DECOMMISSIONING_TIMEOUT;  // keep the button pressed to decommission

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

  Serial.println("\n\nProject: MatterSmartButton");
  Serial.printf("        Board: %s\n", TITLE);
  #ifdef ANTENNA
  Serial.printf("      Antenna: %s\n", ANTENNA);
  #endif
  Serial.printf("Wi-Fi STA MAC: %s\n", STA_MAC_STR);
  Serial.printf("Bluetooth MAC: %s\n", BT_MAC_STR);
  #if CONFIG_ENABLE_CHIPOBLE
  Serial.println("Commissioning: Bluetooth LE");
  #endif
  Serial.println();
  
  // Initialize the USER BUTTON (Boot button) GPIO that will act as a smart button or to decommission the Matter Node
  pinMode(buttonPin, INPUT_PULLUP);


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

  // Initialize the Matter EndPoint
  SmartButton.begin();

  // Matter beginning - Last step, after all EndPoints are initialized
  Matter.begin();
  // This may be a restart of a already commissioned Matter accessory
  if (Matter.isDeviceCommissioned()) {
    Serial.println("Matter Node is commissioned and connected to the network. Ready for use.");
  } else {
    // https://docs.espressif.com/projects/arduino-esp32/en/latest/matter/matter.html
    Serial.printf("isWiFiConnected: %s\n", (Matter.isWiFiConnected()) ? "true" : "false");
    Serial.printf("isThreadConnected: %s\n", (Matter.isThreadConnected()) ? "true" : "false");
    Serial.printf("isDeviceConnected: %s\n", (Matter.isDeviceConnected()) ? "true" : "false");
    Serial.printf("isBLECommissioningEnabled(): %s\n", (Matter.isBLECommissioningEnabled()) ? "true" : "false");
  }
}

void loop() {
  // Check Matter Accessory Commissioning state, which may change during execution of loop()
  if (!Matter.isDeviceCommissioned()) {
    Serial.println("");
    Serial.println("Matter Node is not commissioned yet.");
    Serial.println("Initiate the device discovery in your Matter environment.");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\r\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\r\n", Matter.getOnboardingQRCodeUrl().c_str());
    // waits for Matter Generic Switch Commissioning.
    uint32_t timeCount = 0;
    while (!Matter.isDeviceCommissioned()) {
      delay(100);
      if ((timeCount++ % 50) == 0) {  // 50*100ms = 5 sec
        Serial.println("Matter Node not commissioned yet. Waiting for commissioning.");
      }
    }
    Serial.println("Matter Node is commissioned and connected to the network. Ready for use.");
  }

  // A builtin button is used to trigger a command to the Matter Controller
  // Check if the button has been pressed
  if (digitalRead(buttonPin) == LOW && !button_state) {
    // deals with button debouncing
    button_time_stamp = millis();  // record the time while the button is pressed.
    button_state = true;           // pressed.
  }

  // Onboard User Button is used as a smart button or to decommission it
  uint32_t time_diff = millis() - button_time_stamp;
  if (button_state && time_diff > debounceTime && digitalRead(buttonPin) == HIGH) {
    button_state = false;  // released
    // builtin button is released - send a click event to the Matter Controller
    Serial.println("User button released. Sending Click to the Matter Controller!");
    // Matter Controller will receive an event and, if programmed, it will trigger an action
    SmartButton.click();
  }

  // Onboard User Button is kept pressed for longer than 5 seconds in order to decommission matter node
  if (button_state && time_diff > decommissioningTimeout) {
    Serial.println("Decommissioning the Generic Switch Matter Accessory. It shall be commissioned again.");
    Matter.decommission();
    button_time_stamp = millis();  // avoid running decommissining again, reboot takes a second or so
  }
}
