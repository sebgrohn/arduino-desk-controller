# Arduino Sit–Stand Desk Controller

This is the Arduino sit–stand Desk Controller that lets you extend your simple sit–stand desk with a timer.
If you have dumb a desk with only two buttons – one for driving up and one for driving down – this Arduino sketch automates your sit–stand cycle by hooking into the signals sent to the control unit of the desk.


## Dependencies

The sketch depends on the following Arduino libraries:

* [EEPROM](https://www.arduino.cc/en/Reference/EEPROM) (standard library)
* [LiquidCrystal](https://www.arduino.cc/en/Reference/LiquidCrystal) (standard library)
* [Standard C++ for Arduino](https://github.com/maniacbug/StandardCplusplus)
* [Bounce2](https://github.com/thomasfredericks/Bounce2)
* [Time](http://www.pjrc.com/teensy/td_libs_Time.html)
* [TimeAlarms](http://www.pjrc.com/teensy/td_libs_TimeAlarms.html)

The standard libraries are bundled together with the Arduino IDE; the others, however, need to be downloaded and unpacked to your sketchbook's library folder: `/path/to/sketchbook/libraries/`.


## Schematics

See [SCHEMATICS.md](SCHEMATICS.md).


## Calibration

To function correctly, the up and down speed of the desk must be configured in the sketch. To find these values, follow the instructions in [CALIBRATE.md](CALIBRATE.md).


## Author & Licence

© 2014–2015 Sebastian Gröhn, licenced under the [GNU GPLv3](LICENCE.txt).
