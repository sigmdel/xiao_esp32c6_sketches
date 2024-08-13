// Main module of wifi_uptime Arduino sketch
// Copyright: see notice in wifi_uptime.ino

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "html.h"
#include "secrets.h"

#if defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "Seeed XIAO ESP32C3"
#elif defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  // Uncomment the following macro to use a connected external antenna.
  //#define USE_EXTERNAL_ANTENNA
  #define TITLE "Seeed XIAO ESP32C6"
#elif defined(ARDUINO_MAKERGO_C3_SUPERMINI)
  #define TITLE "MakerGO C3 SuperMini"
  #define TX_POWER WIFI_POWER_17dBm
#endif

#ifndef TITLE
 #error "Unspecified TITLE"
#endif

unsigned long connectTimer = 0;
String connectStatus = "0";

unsigned long bcount = 0;
String buttonStatus = "0";

int rssi = -100;
String rssiStatus = "-100";

// Webserver instance using default HTTP port 80
AsyncWebServer server(80);

// Template substitution function
String processor(const String& var){
  if (var == "UPTIME") return connectStatus;
  if (var == "BCOUNT") return buttonStatus;
  if (var == "RSSI") return rssiStatus;  
  if (var == "SERVERNAME") return String(TITLE);
  return String(); // empty string
}

// 404 error handler
void notFound(AsyncWebServerRequest *request) {
  request->send_P(404, "text/html", html_404, processor);
}

void setup() {
  Serial.begin();
  delay(2000);      // 2 second delay should be sufficient

  Serial.println();
  Serial.printf("Testing Wi-Fi uptime with %s ", TITLE);

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

    Serial.print("using ");
    #ifdef USE_EXTERNAL_ANTENNA
      Serial.print("an external");
    #else
      Serial.print("the internal");
    #endif
    Serial.println(" antenna.");
  #endif

  Serial.println();

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

  connectTimer = millis();
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
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Web button pressed");
    bcount++;
    buttonStatus = bcount;

    connectStatus = (float)  ((float)(millis() - connectTimer)/60000);
    rssi = WiFi.RSSI();
    rssiStatus = rssi;
    request->send_P(200, "text/html", html_index, processor); // updates the client making the request only
  });
  server.onNotFound(notFound);
  server.begin();

  Serial.println("setup completed.");
}

void loop() {
}
