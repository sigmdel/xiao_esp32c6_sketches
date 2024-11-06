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
