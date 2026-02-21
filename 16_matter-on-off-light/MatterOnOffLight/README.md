# Matter On/Off Light Example

This example demonstrates how to create a Matter-compatible on/off light device using a XIAO ESP32-C6 SoC microcontroller.
The application showcases Matter commissioning via Bluetooth, device control via smart home ecosystems, and manual control using a physical button with state persistence.

## Note on Commissioning:

**ESP32-C6** Although it has Thread support, the ESP32 Arduino Matter Library has been pre-compiled using Wi-Fi only. See [original README](README-org.md) for further details.

## Features

- Matter protocol implementation for an on/off light device
- Support for Wi-Fi connectivity
- Simple on/off control
- State persistence using `Preferences` library
- Button control for toggling light and factory reset
- Matter commissioning via QR code or manual pairing code
- Integration with Apple HomeKit, Amazon Alexa, and Google Home

## Hardware Requirements

- ESP32-C6 compatible development board
- LED connected to GPIO pin  for visual feedback (uses the built-in LED by default)
- User button for manual control (uses BOOT button by default)

## Configuration

The default *User configuration* at the top of the `main.cpp` file will work as is for the XIAO ESP32C6 using
the onboard ceramic antenna. The option for using an external antenna and different I/O pins for the LED
and button should be clear.

### Prerequisites

  - Arduino IDE (2.0 or newer) with esp32 by Espressif version 3.3.6 or newer (see `MatterOnOffLight.ino` for details)

  OR

  - pioarduino IDE (v1.2.5 or newer) with pioarduino-espressif 55.03.36 or newer (see `plaformio.ini` for details)
  
Select the **"Huge APP (3MB No OTA/1MB SPIFFS)"** partition. This is specified in the `platformio.ini` configuration file if using the pioarduino IDE extension. In the Arduino IDE,  this must be done manually in the **Tools**  menu.

## Expected Output

As the sketch begins, the following will be displayed in the serial monitor.

```
      Project: MatterOnOffLight
        Board: Seeed XIAO ESP32C6
      Antenna: Onboard ceramic  
Wi-Fi STA MAC: 55:44:33:22:11:10
Bluetooth MAC: 55:44:33:22:11:12

isWiFiConnected: false
isThreadConnected: false
isDeviceConnected: false
isBLECommissioningEnabled(): true

Matter Node is not commissioned yet.
Initiate the device discovery in your Matter environment.
Commission it to your Matter hub with the manual pairing code or QR code
Manual pairing code: 34970112332
QR code URL: https://project-chip.github.io/connectedhomeip/qrcode.html?data=MT:Y.K9042C00KA0648G00
Matter Node not commissioned yet. Waiting for commissioning.
Matter Node not commissioned yet. Waiting for commissioning.
...
```

It was then possible to commission the board using the Alexa app on an Android device via a 4th generation Echo Dot. Commissioning was done with Bluetooth so the ESP32C6 must be close enough to the Echo. Commissioning consists of selecting the Wi-Fi network to which the Echo is connected and entering the network password. However there are a few extra steps such as providing the manual pairing code and accepting to go ahead even if the device is not officially recognized as a Matter certified product. Of course the details will be different depending on the Matter controller used. 

Once commissioning was done, the following message was displayed on the serial monitor by the ESP32C6.

```
Initial state: ON
User Callback :: New Light State = ON
Matter Node is commissioned and connected to the network. Ready for use.
```

That was followed a number of errors.

```
E (666679) chip[DL]: Failed to get configured network when updating network status: Error ESP32:0x0500300F
E (666680) wifi:sta is connecting, return error
E (666682) chip[DL]: esp_wifi_connect() failed: ESP_ERR_WIFI_CONN
E (666688) wifi:sta is connecting, return error
E (666692) chip[DL]: esp_wifi_connect() failed: ESP_ERR_WIFI_CONN
E (694183) chip[SC]: The device does not support GetClock_RealTimeMS() API: 6c.  Falling back to Last Known Good UTC Time
E (694417) chip[DMG]: Fail to retrieve data, roll back and encode status on clusterId: 0x0000_0046, attributeId: 0x0000_FFFCerr = 5c3
```

These could be considered warnings, as from then on it was possible to turn the LED on and off from the Alexa App. This is what the ESP32C6 firmware displays as this is done.

```
User Callback :: New Light State = OFF
User Callback :: New Light State = ON
```

And it was possible to turn the LED on and off with the BOOT button. The firmware indicates that the user button is the source of the command in that case.

```
User button released. Toggling Light!
User Callback :: New Light State = OFF
User button released. Toggling Light!
User Callback :: New Light State = ON
```

See the [original README](README-org.md) for further details.
