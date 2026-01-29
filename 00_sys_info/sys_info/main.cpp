/*
 *  See sys_info.ino for license and attribution.
 */

#include <Arduino.h>

/////// User configuration //////
///
///  Define this to disable the RF selector switch and hence disable the Wi-Fi antenna
///#define DISABLE_WIFI_ANTENNA
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA
///
///  Rate of USB to Serial chip if used on the development board.
///  This is ignored when the native USB peripheral of the 
///  ESP SoC is used such as on the XIAO ESP32C6
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

//---- Basic requirements ----

#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #warning Version 3.3.6 or newer of ESP32 Arduino core version is available
#endif

//---- This sketch could run on most ESP32 boards ----
//---- Identify some ESP32 board ----

#if defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "XIAO ESP32C3"
#elif defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "XIAO ESP32C5"
#elif defined(ARDUINO_XIAO_ESP32C6)
  #define TITLE "XIAO ESP32C6"
#elif defined(ARDUINO_XIAO_ESP32S3)
  #define TITLE "XIAO ESP32S3"
#else 
  #define TITLE "Unknown ESP32 board"
#endif          

  
void printFactoryMac(void) {
  union {
    uint64_t factmac;
    char bytes[sizeof(factmac)];
   } mac;

  #ifdef CONFIG_SOC_IEEE802154_SUPPORTED
	int n = sizeof(uint64_t);  
  #else
  int n = 6;
  #endif
  mac.factmac = ESP.getEfuseMac();
  for (int i=0; i<n; i++) {
    Serial.printf("%02x", mac.bytes[i]);
    if (i<n-1) Serial.print(":"); else Serial.println();
  }
}

void printFlashChipMode(FlashMode_t mode) {
  switch (mode) {
    case FM_QIO:  Serial.print("FM_QIO"); break;
    case FM_QOUT: Serial.print("FM_QOUT"); break;
    case FM_DIO:  Serial.print("FM_DIO"); break;
    case FM_DOUT: Serial.print("FM_DOUT"); break;
    case FM_FAST_READ: Serial.print("FM_FAST_READ"); break;
    case FM_SLOW_READ: Serial.print("FM_SLOW_READ"); break;
	default: Serial.print("FM_UNKNOWN");
  }
  Serial.printf(" (%d)", mode);
}

void getInfo(void) {
  Serial.println("\n\nSystem Information");

	Serial.printf("\nChip model: %s, Revision: %d\n", ESP.getChipModel(), ESP.getChipRevision());
	Serial.printf("  Core count: %d \n", ESP.getChipCores());
	Serial.printf("  CPU frequency: %lu MHz\n", ESP.getCpuFreqMHz());
	Serial.printf("  Cycle count: %lu\n", ESP.getCycleCount());
  Serial.printf("  SDK version: %s\n", ESP.getSdkVersion());

	Serial.println("\nFlash Memory");
	Serial.printf("  Flash size: %lu\n", ESP.getFlashChipSize());
	Serial.printf("  Flash speed: %lu\n", ESP.getFlashChipSpeed());
	Serial.print("  Flash mode: ");
	printFlashChipMode(ESP.getFlashChipMode());
  Serial.println();

	Serial.println("\nPseudo Static Random Access Memory (PSRAM)");
  
  bool hasPSRAM = false;
  #if defined(BOARD_HAS_PSRAM)
    hasPSRAM = true;
  #endif    
	
  unsigned long psramSize = ESP.getPsramSize();
    Serial.printf("  PSRAM size: %lu\n", psramSize);
  if (psramSize) {
	  Serial.printf("  Free PSRAM: %lu\n", ESP.getFreePsram());
	  Serial.printf("  Min free PSRAM: %lu\n", ESP.getMinFreePsram());
	  Serial.printf("  Max PSRAM alloc size: %lu\n", ESP.getMaxAllocPsram());
    if (!hasPSRAM) {
      Serial.println("  *** PSRAM size > 0, but the BOARD_HAS_PSRAM macro is not defined ***");  
    }
  } else if (hasPSRAM) {
    Serial.println("  *** BOARD_HAS_PARAM defined, but ESP.getPsramSize() return 0 ***");
  }

  Serial.println("\nSketch");
  Serial.printf("  Size: %lu\n", ESP.getSketchSize());
	Serial.printf("  Free space: %lu\n", ESP.getFreeSketchSpace());

	Serial.println("\nHeap");
	Serial.printf("  Size: %lu\n", ESP.getHeapSize()); //total heap size
	Serial.printf("  Free: %lu\n", ESP.getFreeHeap()); //available heap
	Serial.printf("  Mininum free since boot: %lu\n", ESP.getMinFreeHeap()); //lowest level of free heap since boot
	Serial.printf("  Maximum allocation size: %lu\n", ESP.getMaxAllocHeap()); //largest block of heap that can be allocated

  #ifdef ARDUINO_XIAO_ESP32C6 
  Serial.println("\nAntenna");
  Serial.printf("  WiFi antenna enabled: %s\n", (digitalRead(WIFI_ENABLE)) ? "false" : "true"); // LOW -> WIFI_ENABLE turns antenna off
  Serial.printf("  Configuration: %s antenna\n", (digitalRead(WIFI_ANT_CONFIG)) ? "external" : "onboard ceramic");
  #endif 
}


void setup() {
  // Delay to allow for the initialization of the native USB peripheral
  // and some time for the IDE to reconnect 
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

  #ifdef ARDUINO_XIAO_ESP32C6
    #ifdef DISABLE_WIFI_ANTENNA
    pinMode(WIFI_ENABLE, OUTPUT);
    digitalWrite(WIFI_ENABLE, HIGH);  
    #endif
    
    #ifdef USE_EXTERNAL_ANTENNA
    pinMode(WIFI_ANT_CONFIG, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH); 
    #endif
  #endif  

  Serial.println("\n\n    Project: sys_info.ino");
  Serial.println("    Purpose: Print information about the SoC and memory");
  Serial.printf("      Board: %s\n", TITLE);
  Serial.print("Factory MAC: ");
  printFactoryMac();
    
}

void loop() {
  getInfo();
	delay(30000);
}
