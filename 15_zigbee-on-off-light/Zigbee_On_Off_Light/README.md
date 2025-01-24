# Arduino-ESP32 Zigbee On/Off Light Example

This example shows how to configure the Zigbee end device and use it as a Home Automation (HA) on/off light.

**The original README was renamed [README-org.md](README-org.md)**

## Supported Targets

Currently, this example supports the following targets.

| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

## Hardware Required

* A development board (ESP32-H2 or ESP32-C6 based) acting as a Zigbee end device (running the Zigbee_On_Off_Light example).

* A USB cable for power supply and programming

* A compatible Zigbee coordinator such as Zigbee2MQTT **or** another development board (ESP32-H2 or ESP32-C6 based) acting as a Zigbee coordinator (running the Zigbee_On_Off_Switch example).

## Configure the Project

Four macros are defined at the top of the source (main.cpp) to handle different development boards. These are
  1. LED_PIN - number of the I/O pin to which a simple LED or an RGB LED is connected.
  1. LED_ON - can be HIGH or LOW depending on which signal turns the LED on. 
  1. SERIAL_BAUD - usually 115200 if the development board has a discrete USB-to-serial adapter and not used otherwise.
  1. USE_EXTERNAL_ANTENNA - define only if the board is an XIAO ESP32C6 and only if an external antenna is used.

The macros are automatically defined correctly for the [Seeed Studio XIAO ESP32C6](https://www.seeedstudio.com/Seeed-Studio-XIAO-ESP32C6-p-5884.html) and should also be correct for the [Espressif Development Kits](https://www.espressif.com/en/products/devkits) although these have not been tested. It may be necessary to tweak some macros for other ESP32-C6 or ESP32-H2 based boards.


## Compiling and Uploading the Sketch

### Using Arduino IDE

Before Compile/Verify, go to the `Tools` menu to modify the following options.

* Select the correct **Board** (example: *"XIAO_ESP32C6"*).
* Select the correct **Port** (examples: *"/dev/ttyACM0"* in Linux, *"Com4"* in Windows).
* Set the **Partition Scheme** to *"Zigbee 4MB with spiffs"*.
* Set the **Zigbee mode** *"Zigbee ED (end device)"*.
* Optional: Set **Core Debug Level** to the desired level such as *"Verbose"* (default is *"None"*).
* Optional: Set **Erase All Flash Before Sketch Upload** to *"Enabled"* (default is *"Disabled"*).

![IDE-Tools-screenshot](../tools_config.jpg)

### Using PlatformIO

*forthcoming*

## Connecting to a Zigbee Coordinator

Assuming that the Zigbee_On_OFF_Light firmware was uploaded to the development board with **Erase All Flash Before Sketch Upload** set to *"Enabled"*, here are the steps to connect to a coordinator for the first time.

### Connecting to a Zigbee2MQTT Coordinator

It is best to ensure that the Zigbee2MQTT coordinator is the nearest Zigbee coordinator to the ESP32 dev board. 

- Open the Zigbee2MQTT web interface (usually at https:[coordinator_ip]:8080/)
- Enable end device connections to the zibgee network by clicking on the **[Permit join (All)]** button in the top menu.
- Power up the development board running the `Zigbee_On_Off_Light` sketch.

After a short while, the board should show up in the Devices table as *Model* `ZBlightBulb`, while the *Manufacturer* is listed as `Unsupported`.

![](../zigbee2mqtt_devices.jpg)

Click on the **Friendly name** so see details about the Zigbee end device and then click on its **Exposes** tab.

![](../zigbee2mqtt_state.jpg)

Clicking on the **OFF** and **ON** buttons in that view controls the LED of the  ESP32 development board running the Zigbee_On_Off_Light sketch. 

Clicking on the boot button of the development board toggles the state of the onboard LED and the State of the end device is updated accordingly in its **Exposes** tab in the Zigbee2MQTT web interface. 


### Connecting to a Zigbee_On_Off_Switch coordinator

It is best to ensure that the development board running the Zigbee_On_Off_Switch coordinator firmware is very near to the ESP32 dev board running the Zigbee_On_Off_Light end device firmware. Tests have shown that a Zigbee2MQTT coordinator or a Zigbee router a few metres away will not interfere.

- Power up the development board running the `Zigbee_On_Off_Light` sketch.
- Power up the development board running the `Zigbee_On_Off_Switch` sketch (network joining is enabled by default).

After a short while, the end device (...Light) should have joined the Zigbee network created by the coordinator (...Switch). The serial output of the two devices should make that clear, especially if the **core debug level** was set to `"Verbose"`. In any case, test by pressing the boot button of the coordinator. It should toggle the state of the LED on the end device.

### Switching between coordinators

It should not be necessary to erase the flash memory of the end device and then to upload its firmware again in order to connect it to a new coordinator, although that should work.

The easiest way to switch between coordinators is to perform a `Zigbee.factoryReset()` of the end device (...Light) and then to reset the device.

  - A `Zigbee.factoryReset()` of the end device (...Light) is performed when its boot button is depressed for more than 3 seconds.

  - A reset of the end device (...Light) is performed by pressing on its Reset button.

Of course, it is important to ensure that the end device is closer to the desired coordinator than to the other coordinator.

The [original README](README-org.md) has more troubleshooting advice.
