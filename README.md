# XIAO ESP32C6 Sketches
**Arduino source code that accompanies [First Look at the Seeed Studio XIAO ESP32C6](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html)**.

## Introduction

The [XIAO ESP32C6](https://www.seeedstudio.com/Seeed-Studio-XIAO-ESP32C6-p-5884.html) is a recent addition to the [Seeed Studio XIAO series](https://www.seeedstudio.com/xiao) of diminutive development boards. It is based on the Espressif ESP32-C6 SoC which is the second Espressif microcontroller based on a RISC-V core.

![](images/ESP32C6_pinout.png)

## PlatformIO Notes

The current Espressif 32 [platform (version 6.9.0 dated Sept 26, 2024)](https://github.com/platformio/platform-espressif32/releases) in PlatformIO (PIO) is based on version 2.0.17 of the ESP32 Arduino core. Consequently it does not support the XIAO ESP32C6 (see [Add board support for Seeed XIAO ESP32C6](https://github.com/platformio/platform-espressif32/pull/1380#issuecomment-2205808510)). 

However, a *fork was created due to the lack of ongoing development for the Espressif 32 Arduino Core for PlatformIO*. Using [pioarduino (p)eople (i)nitiated (o)ptimized (arduino)](https://github.com/pioarduino/platform-espressif32) and a *homebrew* [board definition file](boards/README.md), it is possible to compile all the projects.

Because of the Arduino sketch naming constraints, the `main.cpp` file of a project is not stored in the default `src` directory. A `src_dir` entry in the `platformio.ini` configuration file provides the name of the directory in which `main.cpp` is found. That will be the name of the Arduino sketch as shown below for the `02_blink_pulse_led` project.

```ini
[platformio]
; Make the Arduino IDE happy (.INO file must be in a directory of the same name)
src_dir = blink_pulse_led
boards_dir = ../boards

[env:seeed_xiao_esp32c6]
platform = http://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip
board = seeed_xiao_esp32c6         
...
```

PlatformIO will "convert" the `blink_pulse_led.ino` sketch file, but that is of no consequence since it contains only comments.

Note that the *stable* branch of the **pioarduino platform** is used is all projects except for the first `01_pin_names`. In that case, the *develop*ment branch is used to test the merge of the `seeed_xiao_esp32c6.json` board definition file into the repository ([Add Seeed XIAO ESP32C6 board definition #46](https://github.com/pioarduino/platform-espressif32/pull/46)).

```ini
[platformio]
; Make the Arduino IDE happy (.INO file must be in a directory of the same name)
src_dir = pin_names

[env:seeed_xiao_esp32c6]
platform = https://github.com/pioarduino/platform-espressif32.git#develop
board = seeed_xiao_esp32c6
...
```

## Arduino IDE Notes

The latest version of the Arduino IDE can be obtained for Windows, Linux and macOS in the [Downloads](https://www.arduino.cc/en/software) page from Arduino. 

Install the latest Espressif ESP32 Arduino core.

 1. Add https://espressif.github.io/arduino-esp32/package_esp32_index.json in the Additional Boards Manager URLS in the Preferences window in the IDE.
  
 1.  Install platform `esp32` by Espressif version 3.0.2 or newer with the Boards Manager

Select the `XIAO_ESP32C6` board in the `Tools` menu of the IDE when compiling a project.

Arduino sketches must have an `.ino` file name extension and must be contained in a directory that has the same name as the Arduino sketch (excluding the extension). Consequently the `01_pin_names` project contains a directory named `pin_names` that in turn contains the Arduino sketch `pin_names.ino`. That sketch is basically empty as it is a long comment only. This is not a problem because the Arduino IDE will import all source files found in the sketch directory. The actual code is in `main.cpp` which is the default name of a PlatformIO project.

![Directory tree](images/dir_tree.jpg) 


## List of Projects      

| Project | NB |Purpose |
| ---     |:---:| --- |
| **01_pin_names** | 1,2 | Shows the content of `pins_arduino.h` |
| **02_blink_pulse_led** | 2 | Using `digitalRead`, `digitalWrite`, `analogWrite` (PWM) |
| **03_scan_wifi** | 2,3 | [WiFiScan](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi/examples/WiFiScan) example from the esp32-arduino core |
| **04_wifi_blackhole** | 4 | Looks at Wi-Fi station connect times |
| **05_wifi_tx_power** | 2,4 | Wi-Fi TX power vs connect time |
| **06_async_web_led**| 1,2,4,5 | Toggles the built-in LED on and off with a Web interface |
| **07_ble_led**| 1,2 | Toggles the built-in LED on and off with a Bluetooth LE app |
| **08_zigbee_switch**| 6 | The [Zigbee_Light_Switch](https://github.com/espressif/arduino-esp32/tree/3.0.2/libraries/ESP32/examples/Zigbee/Zigbee_Light_Switch) example from the esp32-arduino core |
| **09_zigbee_bulb**  | 6,7 | Modified [Zigbee_Light_Bulb](https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/Zigbee/Zigbee_Light_Bulb) example from the esp32-arduino core |
| **10_deep_sleep_tmr** | | Deep sleep with timed wake up |
| **11_deep_sleep_io** | 8 | Deep sleep with wake up on I/O event |
| **12_xiao32c6_antenna** | | Examines the I/O configuration for the antenna RF switch |
| **13_wifi_uptime** | 5 | Comparing Wi-Fi connectivity across boards|
| **14_zigbee_on_off_switch**| 6 | The [Zigbee_On_Off_Switch](https://github.com/espressif/arduino-esp32/tree/3.1.0-RC3/libraries/Zigbee/examples/Zigbee_On_Off_Switch) example from the esp32-arduino core |
| **15_zigbee_on_off_light**  | 6,7 | Modified [Zigbee_Light_Bulb](https://github.com/espressif/arduino-esp32/tree/3.1.0-RC3/libraries/Zigbee/examples/Zigbee_On_Off_Light) example from the esp32-arduino core |

NB:
  1. Similar to sketch in [xiao_esp32c3_sketches](https://github.com/sigmdel/xiao_esp32c3_sketches)
  2. Similar to sketch in [supermini_esp32c3_sketches](https://github.com/sigmdel/supermini_esp32c3_sketches)
  3. Released under the GNU LESSER GENERAL PUBLIC LICENSE.
  4. Create a `secret.h` file with the correct Wi-Fi credentials using the `secrets.h.template` as a model. 
  5. Need to install two libraries. The details are in [libraries/README.md](libraries/README.md).
  6. Released under the Apache License, Version 2.0. 
  7. Added support for some XIAO ESP32C6 features.
  8. Requires an external pull up resistor in [most cases](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html#deep_sleep).


### About `08_zigbee_switch` and `09_zigbee_bulb` (Deprecated) 

Flash the Zigbee_Light_Switch firmware on one XIAO ESP32C6 and the Zigbee_Light_Bulb firmware on a second XIAO ESP32C6. The boot button on the first board will toggle the yellow LED on/off on the second board. 

The Zigbee_Light_Bulb board will pair with a Zigbee2MQTT coordinator although it is not supported by the latter. 

With the August 6, 2024 correction that enables the RF switch and properly selects an antenna, it is no longer necessary to reduce the link quality threshold in the Zigbee_Light_Bulb sketch. Nevertheless, the LQI_THRESHOLD macro remains in the code.


### About `14_zigbee_on_off_switch` and `15_zigbee_on_off_light` 

Flash the Zigbee_On_Off_Switch firmware on one XIAO ESP32C6 and the Zigbee_On_Off_Light firmware on a second XIAO ESP32C6. The boot button on the first board will toggle the yellow LED on/off on the second board. 

The Zigbee_On_Off_Light board will pair with a Zigbee2MQTT coordinator although it is not supported by the latter. It is possible to toggle the yellow LED on/off from the Zigbee2MQTT web interface.


### Further Details:

See [First Look at the Seeed Studio XIAO ESP32C6](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html) for some details about these programs.

## Change Log

| Date |     |
| ---  |  ---  |
| 2024-11-23 | Add new Zigbee examples |
| 2024-11-06 | PlatformIO support |
| 2024-08-12 | Added Wi-Fi uptime test |
| 2024-08-09 | Corrected and completed RF switch and antenna selection fix |
| 2024-08-09 | Made the project self-contained by adding private copies of required libraries. |
| 2024-08-06 | Fixed RF switch enable and antenna selection for ESP32 Arduino 3.0.2 and up|
| 2024-08-05 | Investigation of I/O ports controlling the antenna RF switch|
| 2024-08-04 | New pin definitions added in ESP32 Arduino 3.0.4|
| 2024-07-18 | Add board pinout|
| 2024-07-09 | Included use of the Serial peripheral in deep sleep examples|
| 2024-07-08 | Added deep sleep examples|
| 2024-07-07 | Added support for an external antenna (for XIAO ESP32C6) in Zigbee examples |
| 2024-07-01 | Set LQI threshold only when LQI_THRESHOLD macro is defined in the  Zigbee Bulb example|
| 2024-06-29 | Improved ble_led|
| 2024-06-28 | Added ble_led sketch based on BLE libraries|
| 2024-06-27 | Added async web led sketch and `libraries` directory|
| 2024-06-26 | Added Zigbee Light Switch and Bulb sketches|
| 2024-06-25 | Updated after version 3.0.2 of `esp32` core released |
| 2024-06-24 | Initial version |

## License

Copyright 2024, Michel Deslierres. No rights reserved. 

While the copyright pertaining to included libraries must be respected, all the code by Michel Deslierres in this repository is in the public domain. In those jurisdictions where this may be a problem, the [BSD Zero Clause License](https://spdx.org/licenses/0BSD.html) applies.
