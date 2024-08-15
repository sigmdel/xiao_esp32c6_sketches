# Purpose

This sketch is the first draft of tests designed to compare the Wi-Fi connectivity of three development boards: 
  1. [XIAO ESP32C3](https://github.com/sigmdel/xiao_esp32c3_sketches), 
  1. [Super Mini C3](https://github.com/sigmdel/supermini_esp32c3_sketches),
  1. [XIAO ESP32C6](https://github.com/sigmdel/xiao_esp32c6_sketches).

The first two are based on the ESP32-C3 microcontroller while the third contains an ESP32-C6. The last two are equipped with an onboard ceramic antenna, while the first has an FPC external antenna connected to the U.FL connector. The wireless communication problems with the Super Mini C3 has already been documented. The Wi-Fi transmit power was reduced to 17 dBm which seemed optimal based on previous tests.

# Preliminary Results

The three boards were placed side by side as far as practical from a Wi-Fi access point while still within the house. The distance between the AP and the boards was about 16 metres with two walls between the devices. After 305 minutes Wi-Fi connection time, the received signal strength indicator values were somewhat surprising.

| Board | RSSI |
| ---   | ---  |
| XIAO ESP32C3 | -63 |
| Super Mini C3 | -83 |
| XIAO ESP32C6 | -88 |
 
Later, after 1172 minutes of uptime, the range of RSSI values had diminished.

| Board | RSSI |
| ---   | ---  |
| XIAO ESP32C3 | -75 |
| Super Mini C3 | -78 |
| XIAO ESP32C6 | -74 |

After almost a day of operation, the devices were remarkably cool. 

| Board | Temp (°C) |
| ---   | ---  |
| XIAO ESP32C3 | 20.7 |
| Super Mini C3 | 19.9 |
| XIAO ESP32C6 | 20.6 |

This is basically ambient temperature.

The test was terminated after 4050 minutes (more than 2 days and 19 hours).

| Board | RSSI |
| ---   | ---  |
| XIAO ESP32C3 | -71 |
| Super Mini C3 | not connected |
| XIAO ESP32C6 | -81 |

Unfortunately, it was not possible to ascertain when the Super Mini disconnected. The test has to be rewritten to better stress the Wi-Fi radios and with data logging for unattended operation.

# Additional Libraries

The `wifi_uptime` sketch is dependent on two libraries

  1. [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
  2. [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

Minimalist copies of the libraries are included in the [../../libraries](../../libraries/README.md) directory. **Please do not redistribute these files and always return to the original source when using the libraries in other projects.**

Each of the libraries is released under the LGPL-3.0 licence. Its terms can be found in `libraries/AsyncTCP/LICENSE`.
