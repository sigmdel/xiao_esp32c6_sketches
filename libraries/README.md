# Additional Libraries

## MACs

Many sketches use the MACs library to display one of the various MAC addresses an ESP can have. 

Since it is derived from an example in the ESP32 Arduino Core by Espressif, it is released under the LGPL-2.1 licence. 

## ESP32Async Libraries 

The `async_web_led` sketch is dependent on two libraries from ESP32Async

  1. [AsyncTCP](https://github.com/ESP32Async/AsyncTCP) version 3.4.10 by ESP32Async
  2. [ESPAsyncWebServer](https://github.com/ESP32Async/ESPAsyncWebServer) version 3.9.6 by ESP32Async

The period of divergent if not disparate development of the async TCP and web server libraries seems to be coming to an end. The me-no-dev repositories have been archived. It is clearly indicated that the project is now in the hands of the ESP32Async group. Mathieu Carbou is actively maintaining both of these new repositories. The group's version of the libraries is compatible with the latest pioarduino ESP 32 platform and the most recent esp32 Arduino core. In other words, there is no real need for local copies of these two libraries. They are provided only as a convenience to those wanting to run the sketches in this repository with a minimum of fuss. 

What is provided is a pared down version without example files and with modified properties to ensure that nothing new or additional is automatically downloaded.

**Please do not redistribute the content of these directories and always return to the original source when using the libraries in other projects.**

Each of the libraries is released under the LGPL-3.0 licence. Its terms can be found in `libraries/AsyncTCP/LICENSE`.

### Arduino IDE

To use the local libraries in the Arduino IDE, it is necessary to configure the Arduino IDE so that it can find the libraries. 

  1. Open the `async_web_led` sketch in the IDE.

  2. Go to the Preferences window of the IDE: **File » Preferences**

  3. Enter the directory containing the `06_async_web_led` directory in  `Sketchbok location:`  ![](location.jpg)  
  It would be a good to jot down the previous sketchbook location beforehand in order to restore it when done with the example sketch.

  4. Click on the `OK` button.

The Arduino IDE will automatically include any libraries found in the `libraries`. If prompted, do not "update" either of these two libraries  when first testing the `async_web_led` sketch.

If the current libraries from the ESP32Async organization are to be used, simply delete the two directories `Async_TCP` and `ESPAsyncWebServer`. Then install the libraries with the Arduino IDE Library manager (Tools/Manage Libraries...). 
Search for `ESP32Async` and install the latest version of both **Async TCP** and **ESP Async WebServer** by ESP32Async. This will recreate two directories in the `libraries` directory assuming that the Sketchbook location has been set as described above.

### pioarduino/PlatformIO IDE

The `platformion.ini` configuration file contains an entry to the local libraries directory: `lib_dir = ../libraries`. If the current libraries from the ESP32Async organization are to be used, remove that entry, and add library dependency values.

```ini
lib_compat_mode = strict
lib_ldf_mode = chain
lib_deps =
  ESP32Async/AsyncTCP
  ESP32Async/ESPAsyncWebServer     
```
