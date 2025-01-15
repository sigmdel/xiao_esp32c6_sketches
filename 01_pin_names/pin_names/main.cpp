// Main module of pin_names.ino
// Copyright: see notice in pin_names.ino

// The XIAO ESP32C6 pin definitions are found in 
// https://github.com/espressif/arduino-esp32/blob/master/variants/XIAO_ESP32C6/pins_arduino.h
// The BOOT_PIN definition for all ESP32s found in
// https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal.h

#include <Arduino.h>

void iopins(void) {
  Serial.println("\n\nXIAO ESP32C6 I/O Pin Names and Numbers (defined in pins_arduino.h)");

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
  
  Serial.println("\nThe symbolic name and corresponding I/O number of the 7 serial pins");
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

  Serial.println("\nAntenna");
  if (ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 4)) {
    Serial.printf("    WIFI_ENABLE = %d  (RF switch power enable I/O)\n", WIFI_ENABLE);
    Serial.printf("WIFI_ANT_CONFIG = %d (RF switch select control I/O)\n", WIFI_ANT_CONFIG);
  } else {
    Serial.printf("RF switch I/O not defined in ESP32 %s\n", ESP_ARDUINO_VERSION_STR);
    Serial.println("  RF switch power enable I/O = 3   (= WIFI_ENABLE     in ESP32 core 3.0.4 or later)");
    Serial.println("RF switch select control I/O = 14  (= WIFI_ANT_CONFIG in ESP32 core 3.0.4 or later");
  }

  #ifdef BOOT_PIN
  Serial.println("\nBoot Button (defined in esp32-hal.h)");
  Serial.printf("BOOT_PIN = %2d\n", BOOT_PIN);
  #else
  Serial.println("BOOT_PIN not defined")
  #endif

  Serial.println("\nOther macros"); 
  Serial.printf("USB_VID          = 0x%04x\n", USB_VID);
  Serial.printf("USB_PID          = 0x%04x\n", USB_PID);
  Serial.printf("USB_MANUFACTURER = \"%s\"\n", USB_MANUFACTURER);
  Serial.printf("USB_PROUCT       = \"%s\"\n", USB_PRODUCT);
  Serial.printf("USB_SERIAL       = \"%s\"\n", USB_SERIAL);
  
  Serial.println("\nBoard macro");
  #ifdef ARDUINO_XIAO_ESP32C6
  Serial.println("ARDUINO_XIAO_ESP32C6 defined");
  #else
  Serial.println("ARDUINO_XIAO_ESP32C6 not defined");
  #ifdef ARDUINO_XIAO_ESP32C3
  Serial.println("ARDUINO_XIAO_ESP32C3 incorrectly defined"); // prior to version 3.0.2 esp32
  #endif
  #endif
}

void setup() {
  Serial.begin();
  delay(1000); // 1 second delay should be sufficient for USB-CDC
  Serial.println("Setup completed");
}

void loop() {
  iopins();
  delay(10000); // wait 10 seconds
}
