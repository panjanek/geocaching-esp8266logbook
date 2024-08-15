# Geocaching Gadget: esp8266logbook
If you are new to geocaching go to https://www.geocaching.com/ .
Each geocaching hide contains small paper logbook where people log their visit using pencil. 
Additionally, geocache can contain some fun objects.
This project is a geocaching gadget: Digital logbook with tiny OLED screen, operated with two push buttons and simple UI.
Geocachers can enter their nick to the digital logbook or browse the list of saved nicks.
This is improved version of simple digital logbook from here: https://github.com/panjanek/geocaching-attiny85logbook
It has bigger OLED screen, better UI, more features and RTC clock.

## Features

* Allows user to input logbook entry (typically nick), max 21 characters using simple UI with two buttons: "select" and "enter"
* Current date is saved for each entry
* Logbook entries saved in permanent EEPROM memory
* Browsing list of saved entries with dates
* Auto-sleep after ~30s of inacvtivity. Time to sleep showed as a shrinking bar at the bottom.
* Wakes on reset button press.
* Low current in sleep mode - 20uA
* Around 25mA when active
* Low battery indicator in OLED ui
* Can be powered from two AA or AAA batteries or CR123 battery (2.8V - 5V) for year at least
* System log containing various information, battery voltage and more
* Service mode after entering secret admin password. In service mode you can:
   * Adjust clock
   * Display statistics: activation counter, uptime, buttons counters and more
   * Displaying system information: Battery voltage, free space, and more
   * Enable WiFi network to download logbook and system log as text files
   * WiFi OTA software update
   * Clear logbook and system log

## Hardware

### parts

1. ESP8266 module with 4MB Flash. For example ESP-07S. Important: without builtin USB! For example: https://www.ebay.com.au/itm/384751580612 or raw module
2. SSD1306 I2C 128x64 OLED screen
3. PCF8523 I2C Real Time Clock
4. 2N2222 NPN Transistor (as a switch for OLED power pin)
5. 10KOhm resistor
6. Three pushbuttons like this https://www.ebay.co.uk/itm/256536546595
7. Serial USB converter for programming (like FT232)

### wiring

wiring diagram pending...

## Software

### dependencies
1. Arduino IDE 2.3+
2. Adafruit SSD1306 OLED library: https://github.com/adafruit/Adafruit_SSD1306
3. Adafruit RTClib: https://github.com/adafruit/RTClib

### customize to your geocache
1. Select language in `#define LANGUAGE LANG_EN` supported `LANG_EN` and `LANG_PL`. Feel free to make pull request with more languages.
2. Set cache name and author in `WELCOME_LINE5`, `WELCOME_LINE6` and `WELCOME_LINE8`
3. Set administrator password in `ADMIN_PASSWORD`
   
### steps to upload code to esp8266
In Arduino IDE:

1. Install esp8266 board plugin: https://github.com/esp8266/Arduino (may require adding Board Manager URLs in File->Preferences. Add Additional Board Managers URL https://arduino.esp8266.com/stable/package_esp8266com_index.json )
2. Set: Tools -> Board -> esp8266 -> Generic ESP8266 Module
3. Set: Tools -> Flash Size -> 4MB (FS:2MB, OTA:~1019KB)
4. Compile the code
5. Connect USB Serial converter: cross TX and RX. VCC, GND. Connect GPIO0 to GND to enter software upload mode
6. Set Tooles -> Port -> To the COM port where your converter showed up
7. Upload

 ## prototyping

![UI](https://github.com/panjanek/geocaching-esp8266logbook/blob/683a2342864a5b175f347cd586ac7b66349ac6dd/img/logbook2-animation.gif "ui")

![Prototype](https://github.com/panjanek/geocaching-esp8266logbook/blob/683a2342864a5b175f347cd586ac7b66349ac6dd/img/logbook2-proto.png "prototype")
