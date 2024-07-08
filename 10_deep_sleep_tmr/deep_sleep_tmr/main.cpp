// Main module of deep_sleep_tmr.ino
// Copyright: see notice in deep_sleep_tmr.ino

/**************************************************************
OPERATION
  Each time the board wakes up from deep sleep, it restarts by 
  executing the setup() function. First it displays the number 
  of restarts since the last reset by slowly blinking out the
  count with the onboard yellow LED. Then, after a five second
  delay, the LED is flashed quickly a few times indicating 
  that the board will be put into deep sleep for another 
  SLEEP_PERIOD.
  
NOTE
   Adding Serial print statements seems to break this. 
*************************************************************/

#include <Arduino.h>

// 15 seccond sleep period in microseconds
#define SLEEP_PERIOD 15000000

// On board yellow LED
const int ledPin = BUILTIN_LED;
const int ledOn = LOW;

// Counter for number restarts stored in non-volatile memory
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
  // Set builtin led pin as an output
  pinMode(ledPin, OUTPUT); 

  // Increment boot count
  bootCount++;

  // Display bootcount with slow LED blinks
  blink(bootCount, 750); 
  
  // Delay for 5 seconds
  delay(5000); 

  // Quick LED blinks announcing start of deep sleep
  blink(5);

  // Configure deep sleep wake-up timer
  esp_sleep_enable_timer_wakeup(SLEEP_PERIOD); 

  // Enter deep sleep
  esp_deep_sleep_start();
}

void loop() {
  // This odd flashing pattern should never be seen because the 
  // device is put in deep sleep in setup() before loop() starts.
  blink(4, 50);
  delay(125);
  blink(2, 100);
  delay(2000);
}
