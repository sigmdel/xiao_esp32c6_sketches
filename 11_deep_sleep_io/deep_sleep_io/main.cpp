/*
 *  See deep_sleep_io.ino for license and attribution.
 */

/**************************************************************
OPERATION
  Each time the board wakes up from deep sleep, it executes the
  setup() function. Near the end of the setup() function, the
  board is put into deep sleep. Consequently the loop() function
  is never executed and anything in the setup() function after 
  when deep sleep is enabled will also not be executed.

  The board is woken from deep sleep mode by setting a LP GPIO 
  pin either HIGH or LOW. A prompt on the serial monitor will 
  give instructions as to which pin to activate and the needed
  signal to wake the board:

    Boot number: 1
    Wakeup was not caused by deep sleep: 0

    *** For this round, wake the SoC from sleep mode by setting the wake pin HIGH ***

    Wake the SoC by setting LP_GPIO0 [D0] HIGH.
    Going to sleep now

NOTE
  The sketch can report the state of the machine to the 
  serial monitor. However, the serial peripheral is shut
  down when the SoC goes into sleep mode. That means 
  that the serial port has to be reopened when comming
  out of sleep mode. The Arduino IDE automatically 
  reconnects to the serial port quickly enough that 
  the messages from the sketch will be seen. In 
  PlatformIO it is necessary to manually reconnect 
  to the SoC built-in USB of the SoC so that it may 
  not be possible to see the messages.

WARNING
  The Deep sleep mode section of the Getting started Wiki for the XIAO ESP32C5 
  @ https://wiki.seeedstudio.com/xiao_esp32c5_getting_started/#deep-sleep-mode
  contains a CAUTION

    The XIAO ESP32-C5 supports GPIO wake-up and timer wake-up. To prevent the 
    loss of hardware debugging capabilities and increased difficulty in firmware 
    flashing during low-power development, it is strongly recommended that the 
    JTAG (MTMS, MTDI, MTCK, MTDO) pins be reserved for dedicated use and not [be]
    employed as wake-up sources for deep sleep mode.

  The XIAO ESP32-C6 and XIAO ESP32-C3 are quite similar to the XIAO ESP32-C5
  when it comes to deep sleep mode. There are no similar cautions in their 
  Getting started Wiki. 

  Presumably, the "increased difficulty in firmware flashing" refers to the need
  to put the board back into bootloader mode to upload a new firmware. Unless 
  one's timing is perfect, meaning tue firmware upload happens to begin when the
  board is awake and printing instructions on the serial monitor, it will be
  necessary to manually enable the bootloader mode no matter which wake up pin 
  is being used. 
  
  Restricting the use of the JTAG pins for their "dedicated use" is valid when one 
  is using hardware debugging.
*************************************************************/

#include <Arduino.h>

//////// User configuration //////
///
///  Settle time in milliseconds after esp_deep_sleep_enable_gpio_wakeup(
#define SETTLE_TIME 250
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
///#define SERIAL_BEGIN_DELAY 8000
///
//////////////////////////////////

#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 4))    
  #error ESP32 Arduino core version 3.3.4 or newer needed
#endif 

#if !defined(SERIAL_BEGIN_DELAY)
  #if defined(PLATFORMIO)
    #define SERIAL_BEGIN_DELAY 5000    // 5 seconds
  #elif (ARDUINO_USB_CDC_ON_BOOT > 0)
    #define SERIAL_BEGIN_DELAY 2000    // 2 seconds
  #else
    #define SERIAL_BEGIN_DELAY 1000    // 1 second
  #endif
#endif 


//---- Identify the ESP32-Cx based board -----

#if defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "Seeed XIAO ESP32C3"
#elif defined(ARDUINO_XIAO_ESP32C6)
  #define TITLE "Seeed XIAO ESP32C6"
#else
  #error Need to add a wake pin definition below
#endif        


//---- RTC ram data ------------------------------
/*
 * Three integer variables that must be stored in the reat time
 * clock ram which does not lose its content during deep sleep 
 * and during the wake up process from deep sleep
 * 
 */

// Wake mode from esp32/tools/esp32c5-libs/3.3.6-RC1/include/esp_hw_support/include/esp_sleep.h
//   typedef enum {
//     ESP_GPIO_WAKEUP_GPIO_LOW = 0,
//     ESP_GPIO_WAKEUP_GPIO_HIGH = 1
//   } esp_deepsleep_gpio_wake_up_mode_t;
// 
#RTC_DATA_ATTR int wakemode = 0;     

// Counter for restarts stored in non-volatile memory
RTC_DATA_ATTR int bootCount = 0;

// Current index of the wake pin
RTC_DATA_ATTR int currentindex = -1; //initially, no gpio pin is active

//---- WAKE PIN DEFINITIONS --------------------------------------
//
// padcount = number of pin/pads that can be used to wake the SoC
// padlables[] array with board labels of the wake pins
// iopins[] corresponding gpio pin number of wake pins
//
#if defined(ARDUINO_XIAO_ESP32C3)
const int padcount = 4;
const char *padlabels[padcount] = {"D0", "D1", "D2", "D3"};
const int iopins[padcount] = { D0, D1, D2, D3};
#endif
//
//
#if defined(ARDUINO_XIAO_ESP32C6)
const int padcount = 7;
const char *padlabels[padcount] = {"LP_GPIO0 [D0]",  
                                   "LP_GPIO1 [D1]",
                                   "LP_GPIO2 [D2])",  
                                   "LP_GPIO4 [MTMS]",
                                   "LP_GPIO5 [MTDI]",    
                                   "LP_GPIO6 [MTCK]", 
                                   "LP_GPIO7 [MTDO]"};
const int iopins[padcount] = {A0, A1, A2, 4, 5, 6, 7};
#endif


void nextindex(void) {
  currentindex++;
  if (currentindex >= padcount) 
    currentindex = 0;
 
  if (!currentindex) {
    wakemode = 1 - wakemode;
    Serial.printf("\n*** For this round, wake the SoC from sleep mode by setting the wake pin %s ***\n", (wakemode) ? "HIGH" : "LOW");
  }  

  int wkpin = iopins[currentindex];
  Serial.printf("\nWake the SoC by setting %s %s.\n", padlabels[currentindex], (wakemode) ? "HIGH" : "LOW");
  
  pinMode(wkpin, (wakemode) ? INPUT_PULLUP : INPUT_PULLDOWN);
  uint64_t mask = 1ULL << wkpin;
  esp_deep_sleep_enable_gpio_wakeup(mask, (esp_deepsleep_gpio_wake_up_mode_t)wakemode); // ESP_GPIO_WAKEUP_GPIO_HIGH) or _LOW;
  delay(SETTLE_TIME);
  Serial.println("Going to sleep now");
}

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    case ESP_SLEEP_WAKEUP_GPIO : Serial.println("Wakeup caused by GPIO"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

//----------------------------------------------

void setup() {
  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
  Serial.println();
  #endif
  
  if (!bootCount) {
    delay(1000);
    Serial.println("\n\nProject: Test waking the SoC from deep sleep with I/O pin");
    Serial.printf("  Board: %s\n\n", TITLE);
    Serial.println("Instructions:");
    Serial.println("  Set each wake pin either HIGH or LOW as instructed to wake");
    Serial.println("  the board from deep sleep.");
  }

  ++bootCount;
  Serial.println("\n\nBoot number: " + String(bootCount));
  print_wakeup_reason();

  nextindex();
  esp_deep_sleep_start();
  delay(50);
  Serial.println("This will never be printed");
}

void loop() { 
  Serial.println("This is the loop talking. Should never happen.");   
};
