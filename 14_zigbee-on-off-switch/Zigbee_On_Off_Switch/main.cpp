/*
 * See Zibbee_On_Off_Switch.ino for license and attribution
 */

#include <Arduino.h>
#include "Zigbee.h"

#include "MACs.h"

#ifndef ZIGBEE_MODE_ZCZR
  #error "Zigbee coordinator mode is not selected in Tools->Zigbee mode"
#endif

//////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  GPIO pin used by a push button that is connected to GROUND.
///  Define here to overide the automatic use of the BOOT button.
///#define BOOT_PIN 9
///
///  Rate of USB to Serial chip if used on the development board.
///  This is ignored when the native USB peripheral of the 
///  ESP SoC is used.
///
#define SERIAL_BAUD 115200
///
///  Time in milliseconds to wait after Serial.begin() in 
///  the setup() function. If not defined, it will be set
///  to 5000 if running in the PlaformIO IDE to manually switch
///  to the serial monitor otherwise to 2000 if an native USB 
///  peripheral is used or 1000 if a USB-serial adpater is used.
///*define SERIAL_BEGIN_DELAY 8000
///
//////////////////////////////////


#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #error ESP32 Arduino core version 3.3.6 or newer needed
#endif 

#if !defined(CONFIG_SOC_IEEE802154_SUPPORTED)
  #error The SoC must support IEEE 802.15.4 (Zigbee)
#endif

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  #define TITLE "Seeed XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA 
    #define ANTENNA "EXTERNAL"
  #else
    #define ANTENNA "ONBOARD CERAMIC"
  #endif
#elif defined(ARDUINO_BOARD)
  #define TITLE ARDUINO_BOARD
#else
  #define TITLE "Unknown ESP32 board"
#endif        


//---- sanity checks -----------------

#if !defined(BOOT_PIN)
  #error BOOT_PIN not defined
#endif

#if !(ARDUINO_USB_CDC_ON_BOOT > 0) && !defined(SERIAL_BAUD)
  #error SERIAL_BAUD not defined
#endif

//</Configuration> ---------------------

/* Zigbee switch configuration */
#define SWITCH_ENDPOINT_NUMBER 5

#define GPIO_INPUT_IO_TOGGLE_SWITCH BOOT_PIN
#define PAIR_SIZE(TYPE_STR_PAIR)    (sizeof(TYPE_STR_PAIR) / sizeof(TYPE_STR_PAIR[0]))

typedef enum {
  SWITCH_ON_CONTROL,
  SWITCH_OFF_CONTROL,
  SWITCH_ONOFF_TOGGLE_CONTROL,
  SWITCH_LEVEL_UP_CONTROL,
  SWITCH_LEVEL_DOWN_CONTROL,
  SWITCH_LEVEL_CYCLE_CONTROL,
  SWITCH_COLOR_CONTROL,
} SwitchFunction;

typedef struct {
  uint8_t pin;
  SwitchFunction func;
} SwitchData;

typedef enum {
  SWITCH_IDLE,
  SWITCH_PRESS_ARMED,
  SWITCH_PRESS_DETECTED,
  SWITCH_PRESSED,
  SWITCH_RELEASE_DETECTED,
} SwitchState;

static SwitchData buttonFunctionPair[] = {{GPIO_INPUT_IO_TOGGLE_SWITCH, SWITCH_ONOFF_TOGGLE_CONTROL}};

ZigbeeSwitch zbSwitch = ZigbeeSwitch(SWITCH_ENDPOINT_NUMBER);

static bool light_state = false;

/********************* Zigbee functions **************************/
static void onZbButton(SwitchData *button_func_pair) {
  if (button_func_pair->func == SWITCH_ONOFF_TOGGLE_CONTROL) {
    // Send toggle command to the light
    Serial.println("Toggling light");
    zbSwitch.lightToggle();
  }
}

static void onLightStateChange(bool state) {
  if (state != light_state) {
    light_state = state;
    Serial.printf("Light state changed to %d\r\n", state);
  }
}

/********************* Periodic task ***************************/
void periodicTask(void *arg) {
  while (true) {
    // print the bound lights every 10 seconds
    static uint32_t lastPrint = 0;
    if (millis() - lastPrint > 10000) {
      lastPrint = millis();
      zbSwitch.printBoundDevices(Serial);
    }

    // Poll light state every second
    static uint32_t lastPoll = 0;
    if (millis() - lastPoll > 1000) {
      lastPoll = millis();
      zbSwitch.getLightState();
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

/********************* GPIO functions **************************/
static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR onGpioInterrupt(void *arg) {
  xQueueSendFromISR(gpio_evt_queue, (SwitchData *)arg, NULL);
}

static void enableGpioInterrupt(bool enabled) {
  for (int i = 0; i < PAIR_SIZE(buttonFunctionPair); ++i) {
    if (enabled) {
      enableInterrupt((buttonFunctionPair[i]).pin);
    } else {
      disableInterrupt((buttonFunctionPair[i]).pin);
    }
  }
}

/********************* Arduino functions **************************/
void setup() {
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

  #if defined(USE_EXTERNAL_ANTENNA) && defined(ARDUINO_XIAO_ESP32C6)
    //pinMode(WIFI_ANT_CONFIG, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\n     Project: Zigbee On/Off Switch");
  Serial.printf("       Board: %s\n", TITLE);
  #ifdef ANTENNA
  Serial.printf("     Antenna: %s\n", ANTENNA);
  #endif
  Serial.printf("IEEE Address: %s\n\n", ZIGBEE_MAC_STR);

  //Optional: set Zigbee device name and model
  zbSwitch.setManufacturerAndModel("Espressif", "ZigbeeSwitch");

  //Optional to allow multiple light to bind to the switch
  zbSwitch.allowMultipleBinding(true);

  zbSwitch.onLightStateChange(onLightStateChange);

  //Add endpoint to Zigbee Core
  Serial.println("Adding ZigbeeSwitch endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbSwitch);

  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);

  // Init button switch
  for (int i = 0; i < PAIR_SIZE(buttonFunctionPair); i++) {
    pinMode(buttonFunctionPair[i].pin, INPUT_PULLUP);
    /* create a queue to handle gpio event from isr */
    gpio_evt_queue = xQueueCreate(10, sizeof(SwitchData));
    if (gpio_evt_queue == 0) {
      Serial.println("Queue creating failed, rebooting...");
      ESP.restart();
    }
    attachInterruptArg(buttonFunctionPair[i].pin, onGpioInterrupt, (void *)(buttonFunctionPair + i), FALLING);
  }

  // When all EPs are registered, start Zigbee with ZIGBEE_COORDINATOR mode
  if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  }

  Serial.println("Waiting for Light to bind to the switch");
  //Wait for switch to bind to a light:
  while (!zbSwitch.bound()) {
    Serial.printf(".");
    delay(500);
  }

  // Optional: List all bound devices and read manufacturer and model name
  std::list<zb_device_params_t *> boundLights = zbSwitch.getBoundDevices();
  for (const auto &device : boundLights) {
    Serial.printf("Device on endpoint %d, short address: 0x%x\r\n", device->endpoint, device->short_addr);
    Serial.printf(
      "IEEE Address: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n", device->ieee_addr[7], device->ieee_addr[6], device->ieee_addr[5], device->ieee_addr[4],
      device->ieee_addr[3], device->ieee_addr[2], device->ieee_addr[1], device->ieee_addr[0]
    );
    char *manufacturer = zbSwitch.readManufacturer(device->endpoint, device->short_addr, device->ieee_addr);
    char *model = zbSwitch.readModel(device->endpoint, device->short_addr, device->ieee_addr);
    if (manufacturer != nullptr) {
      Serial.printf("Light manufacturer: %s\r\n", manufacturer);
    }
    if (model != nullptr) {
      Serial.printf("Light model: %s\r\n", model);
    }
  }

  Serial.println();

  xTaskCreate(periodicTask, "periodicTask", 1024 * 4, NULL, 10, NULL);
}

void loop() {
  // Handle button switch in loop()
  uint8_t pin = 0;
  SwitchData buttonSwitch;
  static SwitchState buttonState = SWITCH_IDLE;
  bool eventFlag = false;

  /* check if there is any queue received, if yes read out the buttonSwitch */
  if (xQueueReceive(gpio_evt_queue, &buttonSwitch, portMAX_DELAY)) {
    pin = buttonSwitch.pin;
    enableGpioInterrupt(false);
    eventFlag = true;
  }
  while (eventFlag) {
    bool value = digitalRead(pin);
    switch (buttonState) {
      case SWITCH_IDLE:           buttonState = (value == LOW) ? SWITCH_PRESS_DETECTED : SWITCH_IDLE; break;
      case SWITCH_PRESS_DETECTED: buttonState = (value == LOW) ? SWITCH_PRESS_DETECTED : SWITCH_RELEASE_DETECTED; break;
      case SWITCH_RELEASE_DETECTED:
        buttonState = SWITCH_IDLE;
        /* callback to button_handler */
        (*onZbButton)(&buttonSwitch);
        break;
      default: break;
    }
    if (buttonState == SWITCH_IDLE) {
      enableGpioInterrupt(true);
      eventFlag = false;
      break;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
