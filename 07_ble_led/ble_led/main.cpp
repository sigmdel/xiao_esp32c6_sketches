/*
 *  See ble_led.ino for license and attribution.
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "MACs.h"


////// User configuration //////
///
///  This is a simplified example. The BLE service UUID can be some more or less valid reserved 
///  UUID from the Bluetooth(R) Assigned Numbers document 
///  @ https://www.bluetooth.com/specifications/assigned-numbers/ 
///
#define SERVICE_UUID "1815" //"00001815-0000-1000-8000-00805F9B34FB"  // Automation IO Service
///#define SERVICE_UUID "1812" //"00001812-0000-1000-8000-00805F9B34FB"  // Human Interface Device Service
///#define SERVICE_UUID "181c" //"0000181c-0000-1000-8000-00805F9B34FB"  // User Data Service
///#define SERVICE_UUID "57a81fc3-3c5f-4d29-80e7-8b074e34888c"  // custom UUID
///
/// Similarly with the characteritic UUID
///
#define CHARACTERISTIC_UUID "2BE2" //"00002BE2-0000-1000-8000-00805F9B34FB"  // Light Output
///#define CHARACTERISTIC_UUID "2BO5" //"00002B05-0000-1000-8000-00805F9B34FB"  // Power 
///#define CHARACTERISTIC_UUID "2eeae074-8955-47f7-9470-73f85112974f"  // custom UUID
///
///  The custom UUIDs were generated at https://www.guidgenerator.com/
///  https://novelbits.io/uuid-for-custom-services-and-characteristics/
///
/// 
#define BLUETOOTH_NAME  "BLE_LED"
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  I/O pin of the LED to be flashed and pulsed, must be defined here if LED_BUILTIN not defined
///#define LED_PIN  LED_BUILTIN
///
///  Signal level to turn the LED ON
///#define LED_ON  LOW
///
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


#ifdef LED_PIN
static uint8_t ledPin = LED_PIN;
#else
static uint8_t ledPin = LED_BUILTIN;   // XIAO yellow LED cathode connected to digital pin
#endif

#ifdef LED_ON
static uint8_t ledOn = LED_ON;       
#else
static uint8_t ledOn = LOW;            // XIAO yellow LED anode connected to 3.3V via 1.5K resistor
#endif

#if defined(ARDUINO_XIAO_ESP32C6)
  #define TITLE "Seeed XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA
    #define ANTENNA "External antenna"
  #else
    #define ANTENNA "Onboard ceramic"
  #endif    
#elif defined(ARDUINO_BOARD)
  #define TITLE ARDUINO_BOARD
#else
  #define TITLE "Unknown ESP32 board"
#endif    

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;

bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected");
      Serial.println("Restart advertising");
      BLEDevice::startAdvertising(); 
    }
};

class MyCharacteristicCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Serial.println("OnWrite entered");
    String value = pCharacteristic->getValue().c_str();
    if (value == "on") {      
      digitalWrite(ledPin, ledOn); 
      Serial.println("Received \"on\" value");
    } else if(value == "off"){
      digitalWrite(ledPin, 1-ledOn);
      Serial.println("Received \"off\" value");
    } else {
      Serial.printf("Received non valid \"%s\" value \n", value.c_str());
    }  
  }

  void onNotify(BLECharacteristic *pCharacteristic) {
    Serial.println("OnNotify entered");
    String value = pCharacteristic->getValue().c_str();
    if (value == "on") {      
      digitalWrite(ledPin, ledOn); 
      Serial.println("Received \"on\" value");
    } else if(value == "off"){
      digitalWrite(ledPin, 1-ledOn);
      Serial.println("Received \"off\" value");
    } else {
      Serial.printf("Received non valid \"%s\" value \n", value.c_str());
    } 
  }
};


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

  Serial.println("\n\nProject: ble_led");
  Serial.println("Purpose: Toggle a LED with a Bluetooth LE application");
  Serial.printf("  Board: %s\n", TITLE);
  #ifdef ANTENNA
  Serial.printf("Antenna: %s\n", ANTENNA);
  #endif
  Serial.printf(" BT MAC: %s\n", BT_MAC_STR);

  Serial.println("Initializing LED");
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1-ledOn);

  Serial.printf("Initializing BLEDevice %s\n", BLUETOOTH_NAME);
  if (!BLEDevice::init(BLUETOOTH_NAME)) {
    Serial.println("BLE initialization failed");
    Serial.println("Will restart in 5 seconds;");
    delay(5000);
    ESP.restart();
  }  

  Serial.println("Creating a BLE server");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  Serial.println("Adding a BLE service");
  BLEService *pService = pServer->createService(SERVICE_UUID);

  Serial.println("Adding a BLE characteristic");
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->setCallbacks(new MyCharacteristicCallback);

  Serial.println("Starting BLE service");
  pService->start();

  Serial.println("Adding an advertiser");
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->setScanResponse(true);

  Serial.println("Starting BLE advertising");
  BLEDevice::startAdvertising();

  Serial.println("\nSetup completed");
}

unsigned long timer = 0;

void loop() {
  /*
  if (millis() - timer > 10000) {
    if (deviceConnected) {
      Serial.println("Send 'on' or 'off' string to control the LED");
    } else {
      Serial.print("Connect to ");
      Serial.print(BLUETOOTH_NAME);
      Serial.print(" (address: ");
      Serial.print(BLEDevice::getAddress().toString().c_str());
      Serial.println(")");
    }  
    timer = millis();
  }
   */
  if (millis() - timer > 10000) {
    int ledStatus = digitalRead(ledPin);  
    digitalWrite(ledPin, 1-ledStatus); // toggle LED
    if (deviceConnected) {
      pCharacteristic->setValue((ledStatus) ? "on" : "off");
      //pCharacteristic->notify();
      pCharacteristic->indicate();
    } else {
      Serial.print("Connect to ");
      Serial.print(BLUETOOTH_NAME);
      Serial.print(" (address: ");
      Serial.print(BLEDevice::getAddress().toString().c_str());
      Serial.println(")");
    }  
    timer = millis();
  }
}  
