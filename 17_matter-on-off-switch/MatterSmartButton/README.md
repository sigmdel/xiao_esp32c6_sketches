# Matter Smart Button Example

This example demonstrates how to create a Matter-compatible smart button (generic switch) device using an ESP32 SoC microcontroller. 
The application showcases Matter commissioning, sending button click events to smart home ecosystems, and triggering automations based on button presses.

## Note on Commissioning:

**ESP32-C6** Although it has Thread support, the ESP32 Arduino Matter Library has been pre-compiled using Wi-Fi only. See [original README](README-org.md) for further details.

## Features

- Matter protocol implementation for a smart button (generic switch) device
- Support for Wi-Fi connectivity
- Button click event reporting to Matter controller
- Button control for triggering events and factory reset
- Matter commissioning via QR code or manual pairing code
- Integration with Apple HomeKit, Amazon Alexa, and Google Home
- Automation trigger support - button presses can trigger actions in smart home apps

## Hardware Requirements

- ESP32-C6 compatible development board
- User button for triggering events (uses BOOT button by default)

## Configuration

The default *User configuration* at the top of the `main.cpp` file will work as is for the XIAO ESP32C6 using
the onboard ceramic antenna. The option for using an external antenna and a different I/O pin for the push button should be clear.

### Prerequisites

  - Arduino IDE (2.0 or newer) with esp32 by Espressif version 3.3.6 or newer (see `MatterOnOffLight.ino` for details)

  OR

  - pioarduino IDE (v1.2.5 or newer) with pioarduino-espressif 55.03.36 or newer (see `plaformio.ini` for details)
  
Select the **"Huge APP (3MB No OTA/1MB SPIFFS)"** partition. This is specified in the `platformio.ini` configuration file if using the pioarduino IDE extension. In the Arduino IDE,  this must be done manually in the **Tools**  menu.

It is better to erase the board's flash memory before uploading the firmware to remove any old network configuration that might interfere. This is easily done in the Arduino IDE by enabling **"Erase All Flash Before Sketch Upload"** option from **Tools** menu. In the pioarduino IDE, the **PlatformIO: Erase Flash and Upload ...** task has to be explicitly invoked from the **Terminal / Run Task... / PlatformIO** menu selection. Alternatively, open a terminal within the IDE and enter the command `pio run --target erase` which will erase the flash memory and then upload the firmware by clicking on the upload icon as usual.


## Expected Output

As the sketch begins, the following will be displayed in the serial monitor.


```
Project: MatterSmartButton
        Board: Seeed XIAO ESP32C6
      Antenna: Onboard ceramic
Wi-Fi STA MAC: 55:44:33:22:11:20
Bluetooth MAC: 55:44:33:22:11:22
Commissioning: Bluetooth LE

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

It was then possible to commission the board using the Alexa app on an Android device via a 4th generation Echo Dot. Commissioning was done with Bluetooth so the ESP32C6 must be close enough to the Echo. Commissioning consists of selecting the Wi-Fi network to which the Echo is connected and perhaps entering the network password. However there are a few extra steps such as providing the manual pairing code and accepting to go ahead even if the device is not officially recognized as a Matter certified product. Of course the details will be different depending on the Matter controller used. 

Once commissioning was done, the following message was displayed on the serial monitor by the ESP32C6.

```
Matter Node is commissioned and connected to the network. Ready for use.
```

On selecting the Wi-Fi network, there were a number of errors were flagged.

```
> E (348146) chip[DL]: Failed to get configured network when updating network status: Error ESP32:0x0500300F
E (348147) wifi:sta is connecting, return error
E (348149) chip[DL]: esp_wifi_connect() failed: ESP_ERR_WIFI_CONN
E (348155) wifi:sta is connecting, return error
E (348159) chip[DL]: esp_wifi_connect() failed: ESP_ERR_WIFI_CONN
E (375633) chip[SC]: The device does not support GetClock_RealTimeMS() API: 6c.  Falling back to Last Known Good UTC Time
E (375936) chip[DMG]: Fail to retrieve data, roll back and encode status on clusterId: 0x0000_0046, attributeId: 0x0000_FFFCerr = 5c3
```

These could be considered warnings, as from then it would have been possible to send Click events to Alexa. This is what the ESP32C6 firmware displays as this is done.

```
User button released. Sending Click to the Matter Controller!
User button released. Sending Click to the Matter Controller!
```

That will not do much on its own without creating a routine in Alexa that is triggered by the Click event from this ESP32C6 and that executes an action such as toggling a device on or off. That device can be another XIAO ESP32C6 running the MatterOnOffLight sketch if already integrated in Alexa as a light device. It does work! 

See the [original README](README-org.md) for further details.
