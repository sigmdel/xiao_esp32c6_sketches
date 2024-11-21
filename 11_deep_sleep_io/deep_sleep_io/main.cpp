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
  Define the USE_SERIAL macro to print statements to the 
  console with the Serial peripheral. This is best done in the 
  Arduino IDE because its serial monitor will automatically
  reconnect with the ESP32-C6 when it wakes from deep sleep
  and reopens its serial port. With the PlaformIO terminal
  it is necessary to reconnect manually whenever the board
  comes out of deep sleep.
*************************************************************/

#include <Arduino.h>

//#define USE_SERIAL 

// Only RTC IO pins 0 to 7 can be used as a source for external 
// wake on the ESP32-C6 which means only pins D0, D1 or D2 
// for that purpose on the XIAO ESP32C6
const int wakeUpPin = 0; 

// On board yellow LED
const int ledPin = BUILTIN_LED;
const int ledOn = LOW;

// Counter for restarts stored in non-volatile memory
RTC_DATA_ATTR int bootCount = 0; 

// Flash LED for count times with ms on and off times
void blink(int count=1, int ms=50) {
  for (int i=0; i<count; i++) {
    digitalWrite(ledPin, ledOn);
    delay(ms);
    digitalWrite(ledPin, 1-ledOn);
    delay(ms);
  }
}  

#if defined(USE_SERIAL)
  // Print the sleep wakeup cause 
  void print_wakeup_reason(void) {
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    switch(wakeup_reason) {
      case ESP_SLEEP_WAKEUP_UNDEFINED : Serial.println("Reset was not caused by exit from deep sleep"); break;
      case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
      case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
      case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
      case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
      case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
      case ESP_SLEEP_WAKEUP_GPIO : Serial.println("Wakeup caused by GPIO (light sleep only on ESP32, S2 and S3)"); break;
      case ESP_SLEEP_WAKEUP_UART : Serial.println("Wakeup caused by UART (light sleep only)"); break;
      case ESP_SLEEP_WAKEUP_WIFI : Serial.println("Wakeup caused by WIFI (light sleep only)"); break;
      case ESP_SLEEP_WAKEUP_COCPU : Serial.println("Wakeup caused by COCPU int"); break;
      case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG : Serial.println("Wakeup caused by COCPU crash"); break;
      case ESP_SLEEP_WAKEUP_BT : Serial.println("Wakeup caused by BT (light sleep only)"); break;
      default : Serial.printf("Unknown wakeup or reset reason (%d)\n",wakeup_reason); break;
    }
  }
#endif

void setup() { 

  #if defined(USE_SERIAL)
  Serial.begin();
  delay(3000);
  Serial.println("\nDeep Sleep I/O Example");
  print_wakeup_reason();
  #endif

  // Set wake up pin as an input 
  // See https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html#deep_sleep
  // for notes about pull up resistors 
  pinMode(wakeUpPin, INPUT_PULLUP);

  // Set builtin led pin as an output
  pinMode(ledPin, OUTPUT); 

  // Increment boot count
  bootCount++;

  #if defined(USE_SERIAL)
  Serial.printf("Boot count: %d\n", bootCount);
  #endif

  // Display bootcount with slow LED blinks
  blink(bootCount, 750); 
  
  // Delay for 5 seconds
  delay(5000); 

  // Configure the I/O wake-up source 
  if (esp_sleep_enable_ext1_wakeup(1 << wakeUpPin, ESP_EXT1_WAKEUP_ANY_LOW) == ESP_OK) {
 
    #if defined(USE_SERIAL)   
    Serial.println("Closing Serial peripheral and going into deep sleep after LED flashes.");
    Serial.printf("Ground I/O pin D%d to wake from the deep sleep.\n", wakeUpPin);
    Serial.flush();
    Serial.end();
    #endif

    // Quick LED blinks announcing start of deep sleep
    blink(5);

    // Enter deep sleep
    esp_deep_sleep_start(); 

  } else {  
 
    #if not defined(USE_SERIAL)
    Serial.begin();
    #endif
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
