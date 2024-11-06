# Adding a Seeed XIAO ESP32C6 Board Definition in PlatformIO 

Currently there is no Seeed XIAO ESP32C6 board definition in the `espressif32/boards` directory. This directory contains a proposed definition `seeed_xiao_esp32c6.json` that can copied to the correct subdirectory of the `platformio` directory. The location of the latter depends on the operating system and where PlatformIO was initially installed. The *default* locations are as follows.

Linux: 
  `~/.platformio/platforms/espressif32/boards/seeed_xiao_esp32c6.json`

In practice this means: `home/<user>/.platformio/...`.

Windows:
  `%HOMEPATH%\.platformio\platforms\espressif32\boards\eeed_xiao_esp32c6.json`

In practice this means: `C:\Users\<user>\.platformio\...`.


**NOTE:**

The above will work, in the sense that the new board will show up in the Project Wizard. However, the additional file will be deleted when the `espressif32` platform is updated or reinstalled.

There is a simple way to add a locally defined custom board definition to a project. See [Custom Embedded Boards](https://docs.platformio.org/en/latest/platforms/creating_board.html#installation) and [boards_dir](https://docs.platformio.org/en/latest/projectconf/sections/platformio/options/directory/boards_dir.html#projectconf-pio-boards-dir). 

Hopefully, a future release of the `espressif32` platform will include a correct board definition and the problem will become moot. In the meantime, this definition [has been proposed](https://github.com/pioarduino/platform-espressif32/pull/46).
