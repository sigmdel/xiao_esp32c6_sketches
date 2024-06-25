# XIAO ESP32C6 Sketches
**Arduino source code that accompanies [First Look at the Seeed Studio XIAO ESP32C6](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html)**.


## Introduction

The XIAO ESP32C6 is a recent addition to the Seeed Studio XIAO series of diminutive development boards. It is based on the Espressif ESP32-C6 SoC which is the second Espressif microcontroller based on a RISC-V core.

## PlatformIO Notes

The current  <a href="https://github.com/platformio/platform-espressif32/releases" target="_blank">platform</a> (version 6.7.0 dated May 14, 2024) for Espressif 32 bit devices available in PlatformIO (PIO) is based on version 2.0.16 of the ESP32 Arduino core. Consequently it does not support the XIAO ESP32C6. For that reason, these sketches cannot be easily compiled in PIO. Nevertheless the directory structure and file names are such that it will be easy to add this support when PIO is updated.


## Arduino IDE Notes

The latest version of the Arduino IDE can be obtained for Windows, Linux and macOS in the [Downloads](https://www.arduino.cc/en/software) page from Arduino.

Install the latest Espressif ESP32 Arduino core.

 1. Add https://espressif.github.io/arduino-esp32/package_esp32_index.json in the Additional Boards Manager URLS in the Preferences window in the IDE.
  
 1.  Install platform `esp32` by Espressif version 3.0.1 or newer with the Boards Manager

Select the `XIAO_ESP32C6` board in the `Tools` menu of the IDE when compiling a project.


Arduino sketches must have an `.ino` file name extension and must be contained in a directory that has the same name as the Arduino sketch (excluding the extension). Consequenty the `01_pin_names` project contains a directory named `pin_names` that in turn contains the Arduino sketch `pin_names.ino`. That sketch is basically empty as it is a long comment only. This is not a problem because the Arduino IDE will import all source files found in the sketch directory. The actual code is in `main.cpp` which is the default name of a PlatformIO project.

![Directory tree](images/dir_tree.jpg) 


## List of Projects      

| Project | Section in *First Look...* |
| ---     | ---                           |
| **01_pin_names** | [4. Pin Numbers and Names](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html#pins) |
| **02_blink_pulse_led** | [5. Blinky, Blinky](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html#blink) |
| **03_scan_wifi** | [6. Internal vs External Antenna](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html#antenna_1) |
| **04_wifi_blackhole** | [7. The Wi-Fi Blackhole](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html#quirk) |
| **05_wifi_tx_power** | [8. Connection Time vs Transmit Power](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c6_intro_en.html#tx_power) |

## Final Remarks

This is a work in progress, with more sketches to be added as time permits.

## License

Copyright 2024, Michel Deslierres. No rights reserved. 

While the copyright pertaining to included libraries must be respected, all the code by Michel Deslierres in this repository is in the public domain. In those jurisdictions where this may be a problem, the [BSD Zero Clause License](https://spdx.org/licenses/0BSD.html) applies.
