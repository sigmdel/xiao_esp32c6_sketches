// Main module of ble_led.ino
// Copyright: see notice in ble_led.ino

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// This is a simplified example which can use custom UUIDs in which case
// the client will probably show the UUID for the service and characteristic
// or it can use some more or less valid reserved UUID from the Bluetooth(R) 
// Assigned Numbers document https://www.bluetooth.com/specifications/assigned-numbers/ 
//
#define USE_CUSTOM_UUIDS

#define BLUETOOTH_NAME  "BLE_LED"

#ifdef USE_CUSTOM_UUIDS
  // Custom UUID for service and characteristic must not conflict with a reserved UUID 
  // that is no number in the XXXXXXXX-0000-1000-8000-00805F9B34FB range
  // Generated at https://www.guidgenerator.com/
  // https://novelbits.io/uuid-for-custom-services-and-characteristics/
  #define SERVICE_UUID        "57a81fc3-3c5f-4d29-80e7-8b074e34888c"
  #define CHARACTERISTIC_UUID "2eeae074-8955-47f7-9470-73f85112974f"
#else 
  #define SERVICE_UUID        "1815" //"00001815-0000-1000-8000-00805F9B34FB"  // Automation IO Service
                            //"1812" //"00001812-0000-1000-8000-00805F9B34FB"  // Human Interface Device Service
                            //"181c" //"0000181c-0000-1000-8000-00805F9B34FB"  // User Data Service
  #define CHARACTERISTIC_UUID "2BE2" //"00002BE2-0000-1000-8000-00805F9B34FB"  // Light Output
                            //"2BO5" //"00002B05-0000-1000-8000-00805F9B34FB"  // Power 
#endif                          

#if defined(BUILTIN_LED)
  const uint8_t ledPin = BUILTIN_LED;
  const uint8_t ledOn = LOW;
#else
  #error "ledPin not defined"
#endif

#if defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  // Uncomment the following macro to use a connected external antenna.
  //#define USE_EXTERNAL_ANTENNA
#else
  #undef USE_EXTERNAL_ANTENNA
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

class WriteCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
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
  #if defined(ARDUINO_ESP32C6)
  Serial.begin();
  delay(2000); // should be enough for the USB CDC to initialize
  #else
  Serial.begin(115200);
  while (!Serial) delay(10);
  #endif

  Serial.println("\nSetup:");

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

  Serial.println("Initializing LED");
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1-ledOn);

  Serial.println("Initializing BLEDevice");
  BLEDevice::init(BLUETOOTH_NAME);

  Serial.println("Creating a BLE server");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  Serial.println("Adding a BLE service");
  BLEService *pService = pServer->createService(SERVICE_UUID);

  Serial.println("Adding a BLE characteristic");
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new WriteCallbacks);

  Serial.println("Starting BLE service");
  pService->start();

  Serial.println("Add advertiser");
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->setScanResponse(true);

  Serial.println("Start BLE advertising");
  BLEDevice::startAdvertising();

  Serial.println("\nSetup completed");
}

unsigned long timer = 0;

void loop() {
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
}  
