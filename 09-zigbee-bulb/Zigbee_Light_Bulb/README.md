# Arduino-ESP32 Zigbee Light Bulb Example

This example shows how to configure the Zigbee end device and use it as a Home Automation (HA) on/off light bulb.

**This example is based on ESP-Zigbee-SDK example esp_zigbee_HA_sample/HA_on_off_light.**

# Supported Targets

Currently, this example supports the following targets.

| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

## Hardware Required

* One development board (ESP32-H2 or ESP32-C6) acting  as Zigbee coordinator (loaded with Zigbee_Light_switch example)
* A USB cable for power supply and programming
* Choose another board (ESP32-H2 or ESP32-C6) as Zigbee end device (loaded with Zigbee_Light_bulb example)

### Configure the Project

Set the LED GPIO by changing the `LED_PIN` definition. By default, the LED_PIN is `RGB_BUILTIN`.
By default, the `neoPixelWrite` function is used to control the LED. You can change it to digitalWrite to control a simple LED.

#### Using Arduino IDE

To get more information about the Espressif boards see [Espressif Development Kits](https://www.espressif.com/en/products/devkits).

* Before Compile/Verify, select the correct board: `Tools -> Board`.
* Select the End device Zigbee mode: `Tools -> Zigbee mode: Zigbee ED (end device)`
* Select Partition Scheme for Zigbee: `Tools -> Partition Scheme: Zigbee 4MB with spiffs`
* Select the COM port: `Tools -> Port: xxx` where the `xxx` is the detected COM port.

## Troubleshooting

If the End device flashed with this example is not connecting to the coordinator, erase the flash before flashing it to the board. It is recommended to do this if you did changes to the coordinator.
You can do the following:

* In the Arduino IDE go to the Tools menu and set `Erase All Flash Before Sketch Upload` to `Enabled`
* In the sketch uncomment function `esp_zb_nvram_erase_at_start(true# 09-zigbee-bulb

The `Zigbee_Light_Bulb` directory contains basically a clone of the example sketch from the `esp32` version 3.0.2 package which is found here in a Linux system:
`~/.arduino15/packages/esp32/hardware/esp32/3.0.2/libraries/ESP32/examples/Zigbee/Zigbee_Light_Bulb`

However it was necessary to make slight adjustments to the code when using that sketch in conjunction with the `Zigbee_Light_Switch` with two ESP32-C6 boards.

## Modifications to the Source

### LED vs RGB_LED

Some modification of the source code was necessary to support the XIAO ESP32-C6 and other board that have an onboard LED but not an RGB LED. Changes are easily identified because of the use of ```#if defined(RGB_BUILTIN)```.

```cpp
#if defined(RGB_BUILTIN)
  #define LED_PIN RGB_BUILTIN
#elif defined(LED_BUILTIN)
  #define LED_PIN LED_BUILTIN
  #define LED_ON LOW           // LOW = I/O pin must be grounded to turn on LED
#else
  #error "NO LED defined"
#endif
```

### 
Note that on the XIAO the LED is turned on by writting LOW to its I/O pin. Set `#define LED_ON HIGH` for boards where setting the I/O pin high turns the LED on.


### Minimum Link Quality Indicator

If Zigbee bulb does not connect to the Zigbee switch, with the following debug output

```
esp_zb_app_signal_handler(): Network steering was not successful (status: ESP_FAIL)
```

then it may be necessary to adust the Link Quality Indicator threshold as suggested by [xiequnan](https://github.com/espressif/esp-zigbee-sdk/issues/363#issuecomment-2160086939). He proposed a value of 32, but matthiasbuettner159 in the next message used a value of 0. 

By default, the LQI threshold is not modified, uncomment the `LQI_THRESHOLD` macro and modify its value as desired. 

```
//#define LQI_THRESHOLD 32
```

If `LQI_THRESHOLD` is defined, the LQI threshold will be set in the  the `esp_zb_task` function.  

```cpp
static void esp_zb_task(void *pvParameters) {
  esp_zb_cfg_t zb_nwk_cfg = ESP_ZB_ZED_CONFIG();
  esp_zb_init(&zb_nwk_cfg);
  esp_zb_on_off_light_cfg_t light_cfg = ESP_ZB_DEFAULT_ON_OFF_LIGHT_CONFIG();
  esp_zb_ep_list_t *esp_zb_on_off_light_ep = esp_zb_on_off_light_ep_create(HA_ESP_LIGHT_ENDPOINT, &light_cfg);
  esp_zb_device_register(esp_zb_on_off_light_ep);
  esp_zb_core_action_handler_register(zb_action_handler);
  esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);
  esp_zb_secur_network_min_join_lqi_set(32);  //<< https://github.com/espressif/esp-zigbee-sdk/issues/363#issuecomment-2160086939

  //Erase NVRAM before creating connection to new Coordinator
  esp_zb_nvram_erase_at_start(true);  //Comment out this line to erase NVRAM data if you are conneting to new Coordinator

  ESP_ERROR_CHECK(esp_zb_start(false));
  esp_zb_main_loop_iteration();
}
```  

<!--
[174755][I][Zigbee_Light_Bulb.ino:113] esp_zb_app_signal_handler(): ZDO signal: NLME Status Indication (0x32), status: ESP_OK
[174773][I][Zigbee_Light_Bulb.ino:113] esp_zb_app_signal_handler(): ZDO signal: NLME Status Indication (0x32), status: ESP_OK
[177802][I][Zigbee_Light_Bulb.ino:103] esp_zb_app_signal_handler(): Joined network successfully (Extended PAN ID: 54:32:04:ff:fe:11:af:cc, PAN ID: 0x2ee5, Channel:13, Short Address: 0x030b)
[186730][I][Zigbee_Light_Bulb.ino:113] esp_zb_app_signal_handler(): ZDO signal: ZDO Device Unavailable (0x3c), status: ESP_OK
-->

## Compiling 

The Troubleshooting guide in the sketch [README.md](Zigbee_Light_Bulb/README.md) filme suggests erasing the flash before uploading the firwmare. 

> You can do the following:
>  - In the Arduino IDE go to the Tools menu and set Erase All Flash Before Sketch Upload to Enabled
>  - In the sketch uncomment function esp_zb_nvram_erase_at_start(true); located in esp_zb_task function.

Presumably, if `esp_zb_nvram_erase_at_start(true);` is not a comment, (it is NOT in the example sketch), the non volatile storage will be erased when the tasks starts and there would not be any need to erase all flash before uploading the sketch. On the other hand, commenting out `esp_zb_nvram_erase_at_start(true);` could make pairing faster.

Do not forget to set the Arduino configuration as instructed before compiling. Here are the settings used with a XIAO ESP32C6

![](tools_config.jpg)

With these settings, and with a LQI_THRESHOLD of 32, the XIAO ESP32-C6 did pair with another XIAO ESP32-C6 running th Zigbee_Light_Switch sketch.
);` located in `esp_zb_task` function.

By default, the coordinator network is open for 180s after rebooting or flashing new firmware. After that, the network is closed for adding new devices.
You can change it by editing `esp_zb_bdb_open_network(180);` in `esp_zb_app_signal_handler` function.

***Important: Make sure you are using a good quality USB cable and that you have a reliable power source***

* **LED not blinking:** Check the wiring connection and the IO selection.
* **Programming Fail:** If the programming/flash procedure fails, try reducing the serial connection speed.
* **COM port not detected:** Check the USB cable and the USB to Serial driver installation.

If the error persists, you can ask for help at the official [ESP32 forum](https://esp32.com) or see [Contribute](#contribute).

## Contribute

To know how to contribute to this project, see [How to contribute.](https://github.com/espressif/arduino-esp32/blob/master/CONTRIBUTING.rst)

If you have any **feedback** or **issue** to report on this example/library, please open an issue or fix it by creating a new PR. Contributions are more than welcome!

Before creating a new issue, be sure to try Troubleshooting and check if the same issue was already created by someone else.

## Resources

The ESP Zigbee SDK provides more examples:
* ESP Zigbee SDK Docs: [Link](https://docs.espressif.com/projects/esp-zigbee-sdk)
* ESP Zigbee SDK Repo: [Link](https://github.com/espressif/esp-zigbee-sdk)

* Official ESP32 Forum: [Link](https://esp32.com)
* Arduino-ESP32 Official Repository: [espressif/arduino-esp32](https://github.com/espressif/arduino-esp32)
* ESP32 Datasheet: [Link to datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
* ESP32-S2 Datasheet: [Link to datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_en.pdf)
* ESP32-C3 Datasheet: [Link to datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
* ESP32-S3 Datasheet: [Link to datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
* Official ESP-IDF documentation: [ESP-IDF](https://idf.espressif.com)
