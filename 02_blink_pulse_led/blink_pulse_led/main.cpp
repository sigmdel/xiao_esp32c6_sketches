/*
 *  See blink_pulse_led.ino for license and attribution.
 */

#include <Arduino.h>

/////// User configuration //////
///
///  I/O pin of the LED to be flashed and pulsed, must be defined here if LED_BUILTIN not defined
///#define LED_PIN  LED_BUILTIN
///
///  Signal level to turn the LED ON
///#define LED_ON  LOW
///
///  I/O pin of a momentary on push button used to switch between pulsing or heartbeat
///#define BUTTON_PIN  BOOT_PIN
///
///  Digital value of the button I/O pin when the button is activated.
///#define BUTTON_ON_VALUE  LOW
///
///  Push button debounce delay in milliseconds
#define DEBOUNCE_DELAY 100 
///
///  Time in milliseconds the LED is on during the heartbeat
#define ON_TIME  80
///
///  Time in millisecods the LED is off during the heartbeat
#define OFF_TIME 840
///
///  The increment or decrement in the duty cycle between levels when pulsing LED
#define DELTA 10
///
///  The initial duty cycle above 0
#define INIT_DELTA 5
///
///  Length in milliseconds of use of a duty cycle value while pulsing LED
#define LEVEL_PERIOD 50 
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

#ifdef BUTTON_PIN
static uint8_t buttonPin = BUTTON_PIN;
#else
static uint8_t buttonPin = BOOT_PIN;
#endif

#ifdef BUTTON_ON_VALUE
static uint8_t buttonOn = BUTTON_ON_VALUE;
#else
static uint8_t buttonOn = LOW;
#endif

unsigned long delaytime = 0;
int delta = DELTA;
int fade = 0;

void pulse(void) {
  if (millis() - delaytime > LEVEL_PERIOD) {
    fade += delta;
    if (fade <= 0) {
      fade = INIT_DELTA;
      delta = DELTA;
    } else if (fade >= 255) {
      fade = 255;
      delta = - DELTA;
    }
    analogWrite(ledPin, fade);
    delaytime = millis();
  }
}


unsigned long beattime = 0;
int beatcount = 0;
int beatdelay = 0;

/* beatcount
  0 = turn on  for short delay
  1 = turn off for short delay
  2 = turn on  for short delay
  3 = turn off for long delay
*/

void heartbeat(void) {
  if (millis() - beattime > beatdelay) {
    
    if ((beatcount & 1) == 1) // if beatcount odd turn LED off 
       digitalWrite(ledPin, 1-ledOn);
    else                      // if beatcount event turn LED on
       digitalWrite(ledPin, ledOn);
    
    if (beatcount >= 3) {
      beatdelay = OFF_TIME;
      beatcount = 0;
    } else {
      beatdelay = ON_TIME;
      beatcount++;
    }
    beattime = millis();
  }
}

bool doHeartBeat = true;

void switchAction(bool automatic) {
  // turn LED off
  pinMode(ledPin, OUTPUT);  // needed to get out of PWM mode
  digitalWrite(ledPin, 1-ledOn);
  doHeartBeat = !doHeartBeat;
  Serial.printf("Switching to %s mode\n", (doHeartBeat) ? "hearbeat" : "pulse");
  if (doHeartBeat) {
    beattime = millis();
    beatcount = 0;
    beatdelay = OFF_TIME;
  } else {
    delaytime = 0;
    delta = 5;
    fade = 0;
  }  
}


bool debouncing = false;   // set true when button first pressed
unsigned long buttontime;  // time when button first pressed
uint8_t buttonState = 0;   // initial state set in next function
uint8_t buttonActive = 0;  // set to 1 whe debouncing finished

void initializeButton() {
  Serial.println("Setting up push button I/O pin as an input");
  Serial.print("Button I/O pin mode set to ");
  #if (BUTTON_ON_VALUE == LOW)
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println("INPUT_PULLUP");
  #else
  pinMode(buttonPin, INPUT_PULLDOWN);
  Serial.println("INPUT_PULLDOWN");
  #endif

  buttonState = digitalRead(buttonPin);
  if (buttonState == buttonOn) {
    Serial.printf("The pin connected to the button was measured as %s.\n", (buttonState) ? "HIGH" : "LOW");
    Serial.printf("This is not compatible with the setting of BUTTON_ON_VALUE = %s.\n", (buttonOn) ? "HIGH" : "LOW");
    Serial.println("Was the button pressed while the button was being initialized?");
    Serial.println("\nThe board will be restarted in 10 seconds.");
    delay(10000);
    ESP.restart();
  }  
  buttonActive = !buttonState;
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

  Serial.println("\n\nProject: blink_pulse_led");
  Serial.println("Purpose: Test digital input/output and pulse width modulation");
  Serial.printf("  Board: %s\n", TITLE);

  Serial.println("\nSetting up LED output I/O pin");
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, 1-ledOn); // turn LED off

  initializeButton();
  
  Serial.println("Completed setup");
  Serial.println("Press the push button to switch between");
  Serial.println(" - turning the LED on and off in a heartbeat pattern");
  Serial.println(" - ramping the intensity of the LED up and down making it pulse.");
}


void loop() {
  if (doHeartBeat)
    heartbeat();
  else
    pulse();

  if (digitalRead(buttonPin) != buttonState) {
      if (!debouncing) {
        debouncing = true;
        buttontime = millis();  // debounce start time
      } else if (millis() - buttontime > DEBOUNCE_DELAY) {
        debouncing = false;
        if (buttonState == buttonActive) {
          // was active (ie down) so now button is inactive (ie released)
          switchAction(false);
        }
        buttonState = digitalRead(buttonPin);
      }
  }    
}
