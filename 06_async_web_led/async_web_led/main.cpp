// Main module of async_web_led PlatformIO/Arduino sketch
// Copyright: see notice in async_web_led.ino

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "html.h"
#include "secrets.h"

#if defined(BUILTIN_LED)
  static uint8_t ledPin = BUILTIN_LED;
  static uint8_t ledOn = LOW;
  #if defined(ARDUINO_XIAO_ESP32C6)
    // The onboard ceramic antenna is used by default.
    // Uncomment the following macro to use a connected external antenna.
    #define USE_EXTERNAL_ANTENNA
    #define TITLE "Seeed XIAO ESP32C6"
  #else
    #undef USE_EXTERNAL_ANTENNA
  #endif    
#elif defined(ARDUINO_XIAO_ESP32C3)
  // Connect an external LED:
  //  The diode's cathode (-, usually the short lead on the flat side of the LED) is connected to GND.
  //  The diode's anode (+, usually the long lead on the round side of the LED) is connected to a
  //  current limiting 240 ohm resistor. The other lead of the resistor is connected to an I/O pin.
  //
  static uint8_t ledPin = D10;
  static uint8_t ledOn = HIGH;
  #define TITLE "Seeed XIAO ESP32C3"
#else
  #error "ledPin not defined"
#endif

#ifndef TITLE
 #error "Unspecified TITLE"
#endif

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
  request->send_P(404, "text/html", html_404, processor);
}

void setup() {
  // Set the digital pin connected to the LED as an output
  pinMode(ledPin, OUTPUT);

  Serial.begin();
  delay(3000);      // 3 second delay should be sufficient

  #if defined(ARDUINO_XIAO_ESP32C6)
    #if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 0, 4)) 
      // reproduce initVariant() from ESP32 v3.0.4
      uint8_t WIFI_ENABLE = 3;
      uint8_t WIFI_ANT_CONFIG = 14;
      // enable the RF switch
      pinMode(WIFI_ENABLE, OUTPUT);
      digitalWrite(WIFI_ENABLE, LOW);
      // select the internal antenna
      pinMode(WIFI_ANT_CONFIG, OUTPUT);
      digitalWrite(WIFI_ANT_CONFIG, LOW);
    #endif
   
    // same code for ESP32 v3.0.2 and up
    #if defined(USE_EXTERNAL_ANTENNA)
      digitalWrite(WIFI_ANT_CONFIG, HIGH);
    #endif

    Serial.print("Using ");
    #ifdef USE_EXTERNAL_ANTENNA
      Serial.print("an external");
    #else
      Serial.print("the internal");
    #endif
    Serial.println(" antenna.");
  #endif

  setLed(0);

  WiFi.mode(WIFI_STA);

  #ifdef TX_POWER
  WiFi.setTxPower(TX_POWER);
  Serial.printf("Setting Wi-Fi Tx power to %d\n", TX_POWER);
  #endif

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup and start Web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Index page requested");
    request->send_P(200, "text/html", html_index, processor);
  });
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Web button pressed");
    toggleLed();
    request->send_P(200, "text/html", html_index, processor); // updates the client making the request only
  });
  server.onNotFound(notFound);
  server.begin();

  setLed(0);
  Serial.println("setup completed.");
}

void loop() {
}
