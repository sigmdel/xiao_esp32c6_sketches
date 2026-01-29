/*
 *  See pin_names.ino for license and attribution.
 */

#include <Arduino.h>

#if !defined(ARDUINO_XIAO_ESP32C6)
  #error This program is meant to run on the XIAO ESP32C6 only
#endif

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #warning Version 3.3.6 or newer of ESP32 Arduino core version is available
#endif

void iopins(void) {
  Serial.println("\n\nXIAO ESP32C6 I/O pins and macros");
  Serial.println("\nThe symbolic name and corresponding I/O number of the 11 digital pins");
  Serial.printf(" D0 = %2d\n", D0);
  Serial.printf(" D1 = %2d\n", D1);
  Serial.printf(" D2 = %2d\n", D2);
  Serial.printf(" D3 = %2d\n", D3);
  Serial.printf(" D4 = %2d\n", D4);
  Serial.printf(" D5 = %2d\n", D5);
  Serial.printf(" D6 = %2d\n", D6);
  Serial.printf(" D7 = %2d\n", D7);
  Serial.printf(" D8 = %2d\n", D8);
  Serial.printf(" D9 = %2d\n", D9);
  Serial.printf("D10 = %2d\n", D10);

  Serial.println("\nThe symbolic name and corresponding I/O number of the 3 analogue pins");
  Serial.printf(" A0 = %d\n", A0);
  Serial.printf(" A1 = %d\n", A1);
  Serial.printf(" A2 = %d\n", A2);
  Serial.println("Note: Ax = Dx for x = 0, 1 and 2");
  
  Serial.println("\nThe symbolic name and corresponding I/O number of the 8 serial pins");
  Serial.printf("  TX = %2d [UART] (=D6)\n", TX);
  Serial.printf("  RX = %2d [UART] (=D7)\n", RX);
  Serial.printf(" SDA = %2d [I2C]  (=D4)\n", SDA);
  Serial.printf(" SCL = %2d [I2C]  (=D5)\n", SCL);
  Serial.printf("  SS = %2d [SPI]  (=D3)\n", SS);
  Serial.printf("MOSI = %2d [SPI]  (=D10)\n", MOSI);
  Serial.printf("MISO = %2d [SPI]  (=D9)\n", MISO);
  Serial.printf(" SCK = %2d [SPI]  (=D8)\n", SCK);
 
  Serial.println("\nOnboard yellow LED");
  Serial.printf("LED_BUILTIN = %d\n", LED_BUILTIN);
  Serial.printf("BUILTIN_LED = %d // backward compatibility\n", BUILTIN_LED);

  #ifdef BOOT_PIN
  Serial.println("\nOnboard boot button");
  Serial.printf("BOOT_PIN = %2d\n", BOOT_PIN);
  #endif

  Serial.println("\nAntenna pins");
  Serial.printf("    WIFI_ENABLE = %d (RF switch power enable I/O)\n", WIFI_ENABLE);
  Serial.printf("WIFI_ANT_CONFIG = %d (RF switch select control I/O)\n", WIFI_ANT_CONFIG);
  
  Serial.println("\nOther macros"); 
  Serial.printf("USB_VID          defined = 0x%04x\n", USB_VID);
  Serial.printf("USB_PID          defined = 0x%04x\n", USB_PID);
  Serial.printf("USB_MANUFACTURER defined = \"%s\"\n", USB_MANUFACTURER);
  Serial.printf("USB_PROUCT       defined = \"%s\"\n", USB_PRODUCT);
  Serial.printf("USB_SERIAL       defined = \"%s\"\n", USB_SERIAL);
  #ifdef LED_BUILTIN
  Serial.printf("LED_BUILTIN      defined = %d\n", LED_BUILTIN);
  #else
    #error LED_BUILTIN should be defined
  #endif
  #ifdef BUILTIN_LED
  Serial.printf("BUILTIN_LED      defined = %d //deprecated, use LED_BUILTIN\n", BUILTIN_LED);
  #else
    #error BUILTIN_LED should be defined
  #endif
  #ifdef BOOT_PIN
  Serial.printf("BOOT_PIN         defined = %d\n", BOOT_PIN);
  #else
    #error BOOT_PIN should be defined in esp32-hal.h
  #endif
  
  Serial.println("\nPackage macro");
  #ifdef ESP32
  Serial.println("ESP32 defined");
  #else
    #error ESP32 should be defined
  #endif

  Serial.println("\nBoard macros");
  #ifdef ARDUINO_XIAO_ESP32C6
  Serial.println("ARDUINO_XIAO_ESP32C6 defined");
  #else
    #error ARDUINO_XIAO_ESP32C6 should be defined
  #endif

  Serial.println("\nNative USB support");
  #ifdef ARDUINO_USB_CDC_ON_BOOT 
  Serial.printf("ARDUINO_USB_CDC_ON_BOOT defined = %d\n", ARDUINO_USB_CDC_ON_BOOT);
  #else
    #error ARDUINO_USB_CDC_ON_BOOT=1 should be defined
  #endif                                                       
}

void setup() {
  // Delay to allow for the initialization of the native USB peripheral
  // and some time for the IDE to reconnect 
  #if defined(PLATFORMIO)
    #define SERIAL_BEGIN_DELAY 5000    // 5 seconds
  #else
    #define SERIAL_BEGIN_DELAY 2000    // 2 second
  #endif 

  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  Serial.println("\n\nProject: pin_names.ino");
  Serial.println("Purpose: Print I/O Pin Names, GPIO Numbers, etc.");
  Serial.println("  Board: XIAO ESP32C6");
}

void loop() {
  iopins();
  delay(15000); // wait 15 seconds
}
