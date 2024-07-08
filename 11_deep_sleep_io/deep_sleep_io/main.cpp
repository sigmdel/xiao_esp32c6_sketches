// Main module of deep_sleep_io.ino
// Copyright: see notice in deep_sleep_io.ino

/**************************************************************
OPERATION
  Each time the board wakes up from deep sleep, it restarts by 
  executing the setup() function. First it displays the number 
  of restarts since the last reset by slowly blinking out the
  count with the onboard yellow LED. Then, after a five second
  delay, the LED is flashed quickly a few times indicating 
  that the board will be put into deep sleep. Grounding the
  wakeUpPin will wake up the board and the cycle will repeat.
  
NOTE
   Adding Serial print statements seems to break this. 
*************************************************************/

#include <Arduino.h>

// In principle D0, D1 or D2 (I/O pins 0, 1, 2) can be  
// grounded to wake the board from deep sleep.
const int wakeUpPin = 0; 

// On board yellow LED
const int ledPin = BUILTIN_LED;
const int ledOn = LOW;

// Counter for restarts stored in non-volatile memory
RTC_DATA_ATTR int bootCount = 0; 

void blink(int count=1, int ms=50) {
  for (int i=0; i<count; i++) {
    digitalWrite(ledPin, ledOn);
    delay(ms);
    digitalWrite(ledPin, 1-ledOn);
    delay(ms);
  }
}  

void setup() { 
  // Set wake up pin as an input 
  // See https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html#deep_sleep
  // for notes about pull up resistors 
  pinMode(wakeUpPin, INPUT_PULLUP);

  // Set builtin led pin as an output
  pinMode(ledPin, OUTPUT); 

  // Increment boot count
  bootCount++;

  // Display bootcount with slow LED blinks
  blink(bootCount, 750); 
  
  // Delay for 5 seconds
  delay(5000); 

  // Configure the I/O wake-up source 
  if (esp_sleep_enable_ext1_wakeup(1 << wakeUpPin, ESP_EXT1_WAKEUP_ANY_LOW) == ESP_OK) {
 
    // Quick LED blinks announcing start of deep sleep
    blink(5);

    // Enter deep sleep
    esp_deep_sleep_start(); 

  } else {  
 
    Serial.begin();
    while (true) {
      delay(2000);
      Serial.println("Cannot go into deep sleep mode");
    }  
 
  }
 
}

void loop() {
  // This odd flashing pattern should never be seen because the 
  // device is put in deep sleep in setup() before loop() starts.
  blink(4, 50);
  delay(125);
  blink(2, 100);
  delay(2000);
}
