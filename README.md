# Geocaching Gadget: esp8266logbook
If you are new to geocaching go to https://www.geocaching.com/ .
Each geocaching hide contains small paper logbook where people log their visit using pencil. 
Additionally, geocache can contain some fun objects.
This project is a geocaching gadget: Digital logbook with tiny OLED screen, operated with two push buttons and simple UI.
Geocachers can enter their nick to the digital logbook or browse the list of saved nicks.
This is improved version of simpled digital logbook from here: https://github.com/panjanek/geocaching-attiny85logbook
It has bigger OLED screen, better UI, more features and RTC clock.

## Features

* Allows user to input logbook entry (typically nick), max 21 characters using simple UI with two buttons: "select" and "enter"
* Current date is saved for each entry
* Logbook entries saved in permanent EEPROM memory
* Browsing list of saved entries with dates
* Auto-sleep after ~30s of inacvtivity. 
* Wakes on reset button press.
* Low current in sleep mode - 20uA
* Around 25mA when active
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

   
