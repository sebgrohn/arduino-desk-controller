# Arduino Sit–Stand Desk Controller

This is the Arduino sit–stand Desk Controller that lets you extend your simple sit–stand desk with a timer.
If you have dumb a desk with only two buttons – one for driving up and one for driving down – this Arduino
sketch automates your sit–stand cycle by hooking into the signals sent to the control unit of the desk.


## Dependencies

The sketch depends on the following Arduino libraries:

| Library                                                                    | Version | Library Manager package                                                                        | Notes                           |
|:---------------------------------------------------------------------------|:--------|:-----------------------------------------------------------------------------------------------|:--------------------------------|
| [EEPROM](https://www.arduino.cc/en/Reference/EEPROM)                       |         | –                                                                                              | Standard library                |
| [LiquidCrystal](https://www.arduino.cc/en/Reference/LiquidCrystal)         | 1.0     | [`LiquidCrystal:1.0.4`](https://github.com/arduino-libraries/LiquidCrystal/releases/tag/1.0.4) | Standard library                |
| [Standard C++ for Arduino](https://github.com/maniacbug/StandardCplusplus) |         | _missing_                                                                                      | Not adapted for Library Manager |
| [Bounce2](https://github.com/thomasfredericks/Bounce2)                     | 2.1     | [`Bounce2:2.1.0`](https://github.com/thomasfredericks/Bounce2/releases/tag/V2.1)               |
| [Time](http://www.pjrc.com/teensy/td_libs_Time.html)                       | 1.5     | [`Time:1.5.0`](https://github.com/PaulStoffregen/Time/releases/tag/v1.5)                       |
| [TimeAlarms](http://www.pjrc.com/teensy/td_libs_TimeAlarms.html)           | 1.4     | [`TimeAlarms:1.4.0`](https://github.com/PaulStoffregen/TimeAlarms/releases/tag/1.4.1)          |
| [BLE SDK for Arduino](https://github.com/Cheong2K/ble-sdk-arduino)         | 1.0     | [``]()               |
| [RBL_nRF8001](https://github.com/RedBearLab/nRF8001)                       | 1.0     | [``]()               |

The standard libraries are bundled together with the Arduino IDE. The others must to be added manually,
either via the [Library Manager](https://www.arduino.cc/en/Guide/Libraries#toc3) (new in Arduino IDE 1.6.2),
or by downloading and unpacking them to your sketchbook's library folder: `/path/to/sketchbook/libraries/`.


## Schematics

See [SCHEMATICS.md](SCHEMATICS.md).


## Calibration

To function correctly, the up and down speed of the desk must be configured in the sketch. To find these
values, follow the instructions in [CALIBRATE.md](CALIBRATE.md).


## Author & Licence

© 2014–2016 Sebastian Gröhn, licenced under the [GNU GPLv3](LICENCE.txt).
