/*
 *  See async_web_led.ino for license and attribution.
 */

#include <Arduino.h>
#include <WiFi.h>

#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "MACs.h"
#include "html.h"
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
///  By default LED_PIN is set to BUILTIN_LED when the latter is defined 
///  Some boards such as the XIAO ESP32C3 do not have a builtin LED and the LED_PIN macro 
///  must specify the GPIO pin to which an external LED is connected.
///#define LED_PIN D0
///
///  Define the signal needed to turn the LED on.
///    #define LED_ON LOW  
///  assumes that the diode's anode (+) is connected to HIGH and the cathode (-) is connected to the GPIO pin
///  If the diode's cathode (-) is connected to GND and the anode (+) is connected to the GPIO pin then 
///  set LED_LOW to HIGH.
///  Do not forget to insert a current limiting resistor (around 240 ohms depending on the LED and desired
///  brightness) in the circuit.
#define LED_ON LOW 
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

#if !defined(LED_PIN) && defined(BUILTIN_LED)
#define LED_PIN BUILTIN_LED
#endif

#if defined(ARDUINO_BOARD)
  #define TITLE ARDUINO_BOARD
  #if defined (ARDUINO_VARIANT)
    #define VARIANT ARDUINO_VARIANT  
  #endif
#elif 
  #define TITLE "Unknown ESP32 board"
#endif  

#ifndef TIMEOUT
#define TIMEOUT 120000
#endif

const int ledPin = LED_PIN;
const int ledOn = LED_ON;

int ledState = 0;
String ledStatus = "OFF";

void setLed(int value) {
  digitalWrite(ledPin, (value)? ledOn : 1-ledOn);
  ledState = value;
  ledStatus = ((digitalRead(ledPin) == ledOn) ? "ON" : "OFF");
  Serial.printf("LED now %s.\n", ledStatus.c_str());
}

void toggleLed(void) {
  setLed(1-ledState);
}

// Webserver instance using default HTTP port 80
AsyncWebServer server(80);

// Template substitution function
String processor(const String& var){
  if (var == "LEDSTATUS") return String(ledStatus);
  if (var == "SERVERNAME") return String(TITLE);
  return String(); // empty string
}

// 404 error handler
void notFound(AsyncWebServerRequest *request) {
  Serial.printf("\"%s\" not found\n", request->url().c_str());
  request->send(404, "text/html", html_404, processor);
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

  Serial.println("\n\nProject: async_web_led");
  Serial.println("Purpose: Run a Web server to control a LED from an HTML page");
  Serial.printf("  Board: %s\n", TITLE);
  #ifdef VARIANT
  Serial.printf("Variant: %s\n", VARIANT);
  #endif
  Serial.printf("STA MAC: %s\n", STA_MAC_STR);
  Serial.printf("Network: %s\n", ssid);

  // Set the digital pin connected to the LED as an output
  pinMode(ledPin, OUTPUT);
  setLed(0);

  WiFi.mode(WIFI_STA);

  #ifdef TX_POWER
  WiFi.setTxPower(TX_POWER);
  Serial.printf("Setting Wi-Fi Tx power to %d\n", TX_POWER);
  #endif

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);

  unsigned long connecttimer = millis();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
    if (millis() - connecttimer > TIMEOUT) {
      Serial.printf("Not connected within %.1f seconds\n", TIMEOUT/1000.0);
      Serial.println("Will restart in 5 seconds");
      delay(5000);
      ESP.restart();
    }  
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup and start Web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Index page requested");
    request->send(200, "text/html", html_index, processor);
  });
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Web button pressed");
    toggleLed();
    request->send(200, "text/html", html_index, processor); // updates the client making the request only
  });
  server.onNotFound(notFound);
  server.begin();

  setLed(0);
  Serial.println("setup completed.");
}

void loop() {
}
