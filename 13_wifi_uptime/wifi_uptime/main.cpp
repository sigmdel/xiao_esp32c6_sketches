/*
 *  See wifi_uptime.ino for license and attribution.
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
///#define TX_POWER WIFI_POWER_17dBm
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

#if defined(ARDUINO_ESP32C3_DEV)
  #define TITLE "ESP32C3 SuperMini"
#elif defined(ARDUINO_BOARD)
  #define TITLE ARDUINO_BOARD
#else
  TITLE "Unknown ESP32 board"
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

  Serial.println("\n\nProject: wifi_uptime");
  Serial.println("Purpose: Run a Web server to check RSSI and WiFi connection time");
  Serial.printf("  Board: %s\n", TITLE);
  #if defined(ARDUINO_XIAO_ESP32C6)
    Serial.print("Antenna: ");
    #if defined(USE_EXTERNAL_ANTENNA) 
      Serial.println("External");
    #else
      Serial.println("Onboard ceramic");
    #endif  
  #endif    
  Serial.printf("STA MAC: %s\n", STA_MAC_STR);
  Serial.printf("Network: %s\n", ssid);
        
  WiFi.mode(WIFI_STA);

  #ifdef TX_POWER
  WiFi.setTxPower(TX_POWER);
  Serial.printf("TX power set to: %d\n",  WiFi.getTxPower());
  #endif


  // Connect to Wi-Fi network with SSID and password
  Serial.printf("\nConnecting to %s\n", ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  connectTimer = millis();
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup and start Web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Index page requested");
    request->send(200, "text/html", html_index, processor);
  });
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Web button pressed");
    bcount++;
    buttonStatus = bcount;

    connectStatus = (float)  ((float)(millis() - connectTimer)/60000);
    rssi = WiFi.RSSI();
    rssiStatus = rssi;
    request->send(200, "text/html", html_index, processor); // updates the client making the request only
  });
  server.onNotFound(notFound);
  server.begin();

  Serial.println("setup completed.");
}

void loop() {
}
