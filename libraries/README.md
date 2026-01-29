# Additional Libraries

Many sketcthes use the MACs library to display one of the various MAC addresses an ESP can have. Since it is derived from an example in the ESP32 Arduino Core by Espressif, it is released under the LGPL-2.1 licence. 

The `async_web_led` sketch is dependant on two libraries

  1. [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
  2. [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

For about five years, the ESPAsyncWebServer library authored by Hristo Gochkov, was not being updated. Over that period it became incompatible with newer versions of the ESP32 Arduino core which prompted many developers to fork the project and update it in a rather haphazard way. It became difficult to locate a correct version to use in that period. The developers of the Arduino IDE and PlatformIO appear to have tried to solve the situation by offering to install many forks of the libraries, without much guidance to the user on which version would be best.

A couple of months ago, version 1.2.4 of ESPAsyncWebServer compatible with ESP32 Arduino 3.0.0 was released. Since it is also compatible with the ESP32-C6, I have decided to use the "root" version of the library. Unfortunately, the Arduino Library Manager does not provide access to that version. Consequently, a manual installation of the libraries is required. While it is not particularly difficult to do, it seemed better to provide a **minimalist copy** of each the libraries in this repository in order to make it self-contained. **Please do not redistribute these files and always return to the original source when using the libraries in other projects.**

It is still necessary to configure the Arduino IDE so that it can find the libraries. 

  1. Open the `async_web_led` sketch in the IDE.

  2. Go to the Preferences window of the IDE: **File » Preferences**

  3. Enter the directory containing `06_async_web_led` directory in  `Sketchbok location:` ![](location.jpg)  
  It would be a good to jot down the previous sketchbook location beforehand in order to restore it when done with the example sketch.

  4. Click on the `OK` button.

The Arduino IDE will automatically include any libraries found in the `libraries`. Be warned that it will want to install "updates" of these two libraries. Many of the offerings proposed by the IDE did not work, but the situation may have evolved since they were tried. It may be wise to forego updating when first testing the `async_web_led` sketch.

Each of the libraries is released under the LGPL-3.0 licence. Its terms can be found in `libraries/AsyncTCP/LICENSE`.
