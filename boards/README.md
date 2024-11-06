# Adding a Seeed XIAO ESP32C6 Board Definition in PlatformIO 

The stable (51.03.07) branch of the [pioarduino](https://github.com/pioarduino/platform-espressif32) platform does not contain a board definition for the XIAO ESP32C6. However, this directory contains a proposed definition `seeed_xiao_esp32c6.json` which has been successfully tested with all the projects in the repository.

To use the [boards/seeed_xiao_esp32c6.json](seeed_xiao_esp32c6.json) definition in a PlatformIO project, add the following line in the project configuration file (`platformio.ini`): `boards_dir = ../boards`. Here is a typical example.

```ini
platformio]
src_dir = async_web_led
boards_dir = ../boards

[env:seeed_xiao_esp32c6]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/51.03.07/platform-espressif32.zip
board = seeed_xiao_esp32c6
framework = arduino
...
```

> References: [Custom Embedded Boards](https://docs.platformio.org/en/latest/platforms/creating_board.html#installation),  [boards_dir](https://docs.platformio.org/en/latest/projectconf/sections/platformio/options/directory/boards_dir.html#projectconf-pio-boards-dir). 


The development branch of the [pioarduino](https://github.com/pioarduino/platform-espressif32) platform contains the board definition since November 6, 2024 ([Commit bdae53d
](https://github.com/pioarduino/platform-espressif32/commit/bdae53d076dce7737f801cad9cba4595be3866a2). The first example project in this repository uses that branch. Here are the first few lines of its  configuration file. 

```ini
[platformio]
; Make the Arduino IDE happy (.INO file must be in a directory of the same name)
src_dir = pin_names

[env:seeed_xiao_esp32c6]
platform = https://github.com/pioarduino/platform-espressif32.git#develop
...
```
In this project, the content of the `../board` directory will be ignored.

## Credit

Just came across [lucaskatayama:feat/seeed_xiao_esp32c6.json](https://github.com/platformio/platform-espressif32/pull/1472/commits/91ed04a0b6e197001d747e651d944a6f25928557) dated 0ct 4th. That board definition looks to be exactly the same as [seed_xiao_esp32c6.json](seeed_xiao_esp32c6.json) in this directory but for the order in which elements are listed.

It's all a bit confusing. Lucas wanted to merge the board definition into [platform-espressif32](https://github.com/platformio/platform-espressif32platform-espressif32). According to [valeros](https://github.com/platformio/platform-espressif32/pull/1380#issuecomment-2205808510), [the ESP32-C6] *requires Arduino core v3.0 which is not officially supported **yet**.* (July 3). Two weeks later jason2866, announced the [pioarduino](https://github.com/pioarduino/platform-espressif32) fork writing *[since] it is **now** clear that there will be no official support for Arduino core 3.0.x from Platformio team, [...] i decided to fork the needed repo(s) and build a community version to support core 3.0.x (starting with core 3.0.3)*. (The emphasis on "yet" and "now" is mine.) So Lucas' work will not be used and I preempted his ability to contribute his definition to the pioarduino team? **My apologies to Lucas Katayama whose work I should have credited.**

Also, many thanks to jason2866 for pioarduino, it works very well.
