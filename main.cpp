// Main module of blink_pulse_led
// Copyright: see notice in blink_pulse.ino

static uint8_t ledPin = LED_BUILTIN;   // yellow LED cathode connected to digital pin
static uint8_t ledOn = LOW;            // the LED anode is connected to 3.3V via 1.5K resistor

static uint8_t buttonPin = 9;  // Boot button
uint8_t buttonState = 0;       // It should be high when read by setup()
uint8_t buttonActive = 0;      // 

void setup() {
  Serial.begin();
  delay(1000); // 1 second delay should be sufficient for USB-CDC
  Serial.println("Starting setup");

  Serial.println("Setting up LED output I/O pin");
  // declaring LED pin as output
  pinMode(ledPin, OUTPUT);
  // turn it off
  digitalWrite(ledPin, 1-ledOn);

  Serial.println("Setting up Boot button input I/O pin");
  // declaring boot button pin as input
  pinMode(buttonPin, INPUT);  // externally pulled high with 10K resistor
  buttonState = digitalRead(buttonPin);
  Serial.printf("Boot button default value: %d\n", buttonState);
  buttonActive = !buttonState;

  Serial.println("Completed setup");
  Serial.println("Executing loop");
}

#define DELTA 10
#define DELAY 50

unsigned long delaytime = 0;
int delta = DELTA;
int fade = 0;

void pulse(void) {
  if (millis() - delaytime > DELAY) {
    fade += delta;
    if (fade <= 0) {
      fade = 0;
      delta = DELTA;
    } else if (fade >= 255) {
      fade = 255;
      delta = - DELTA;
    }
    analogWrite(ledPin, fade);
    delaytime = millis();
  }
}


#define ON_TIME  80
#define OFF_TIME 840

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

unsigned long timer = 0;
bool doHeartBeat = true;

void switchAction(bool automatic) {
  // turn LED off
  pinMode(ledPin, OUTPUT);  // needed to get out of PWM mode
  digitalWrite(ledPin, 1-ledOn);
  doHeartBeat = !doHeartBeat;
  Serial.printf("Switching to %s mode %s\n", 
    (doHeartBeat) ? "hearbeat" : "pulse", 
    (automatic) ? "automatically" : "after button press"
  );   
  if (doHeartBeat) {
    beattime = millis();
    beatcount = 0;
    beatdelay = OFF_TIME;
  } else {
    delaytime = 0;
    delta = 5;
    fade = 0;
  }  
  timer = millis();
}

#define AUTOSWITCH_DELAY 20000  // ms = 20 seconds
#define DEBOUNCE_DELAY 100 // ms
bool debouncing = false;
unsigned long buttontime = 0;

void loop() {
  if (doHeartBeat)
    heartbeat();
  else
    pulse();

  if (millis() - timer > AUTOSWITCH_DELAY) {
    switchAction(true);
  } else if (digitalRead(buttonPin) != buttonState) {
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
  // do other stuff here
}
