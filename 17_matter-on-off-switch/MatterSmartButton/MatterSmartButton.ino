/*
 * MatterSmartButton.ino
 * Demonstrates how to create a Matter-compatible remote on/off switch using an ESP32-C6 SoC microcontroller
 *
 * This is a stub to satisfy the Arduino IDE. The source code is 
 * in the file main.cpp in the same directory.
 *
 * This sketch will compile in the Arduino IDE
 *
 *  1- Add https://espressif.github.io/arduino-esp32/package_esp32_index.json
 *     in the Additional Boards Manager URLS in the Preferences window.
 *  2- Install platform esp32 by Espressif version 3.3.6 or newer with the Boards Manager
 *  3- Select the XIAO ESP32C6 board
 *  4. Set the Partition Scheme  to "Huge APP (3MB No OTA/1MB SPIFFS)" in the Tools menu.
 *
 * Michel Deslierres (https://github.com/sigmdel)
 * February 21, 2026
 *
 */

// From the original source: 
//
// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 * Slightlty modified to support the XIAO ESP32C6 and perhaps other ESP32-C6 based boards
 *   - support for active LOW LED added
 *   - support for the XIAO ESP32C6 external antenna added
 *
 */
