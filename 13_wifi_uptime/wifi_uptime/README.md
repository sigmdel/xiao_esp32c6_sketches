# Purpose

This sketch is the first draft of tests designed to compare the Wi-Fi connectivity of three development boards: 
  1. [XIAO ESP32C3](https://github.com/sigmdel/xiao_esp32c3_sketches), 
  1. [Super Mini C3](https://github.com/sigmdel/supermini_esp32c3_sketches),
  1. [XIAO ESP32C6](https://github.com/sigmdel/xiao_esp32c6_sketches).

The first two are based on the ESP32-C3 microcontroller while the third contains an ESP32-C6. The last two are equipped with an onboard ceramic antenna, while the first has an FPC external antenna connected to the U.FL connector. The wireless communication problems with the Super Mini C3 has already been documented. The Wi-Fi transmit power was reduced to 17 dBm which seemed optimal based on previous tests.

# Preliminary Results

The three boards were placed side by side as far as practical from a Wi-Fi access point while still within the house. The distance between the AP and the boards was about 16 metres with two walls between the devices. The received signal strength indicator values were transcribed into the following table at four different times over almost 3 days.

| Board | 305 min |  1172 min | 4050 min | 4237 min | 
| ---   |:---: | :---: |  :---: |  :---: |
| XIAO ESP32C3 | -63 | -75 | -71 | -67 |
| Super Mini C3 | -83 | -78 | n.c. | -83 |
| XIAO ESP32C6 | -88 | -74 | -81 | -89 |
 
Here n.c. indicates that the Super Mini was not connected. Not only could the web page not be reached, but the device was not even listed among the Wi-Fi clients by the router. Nevertheless, the board reestablished the connection. 

After almost a day of operation, the devices were remarkably cool. 

| Board | Temp (°C) |
| ---   | ---  |
| XIAO ESP32C3 | 20.7 |
| Super Mini C3 | 19.9 |
| XIAO ESP32C6 | 20.6 |

This is basically ambient temperature.

Clearly, the test has to be rewritten to better stress the Wi-Fi radios and with data logging for unattended operation.

# Additional Libraries

The `wifi_uptime` sketch is dependent on two libraries

  1. [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
  2. [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

Minimalist copies of the libraries are included in the [../../libraries](../../libraries/README.md) directory. **Please do not redistribute these files and always return to the original source when using the libraries in other projects.**

Each of the libraries is released under the LGPL-3.0 licence. Its terms can be found in `libraries/AsyncTCP/LICENSE`.
