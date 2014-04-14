Motor Vibrometer
================

Measures vibrations of copter motor<br>
Project requres [i2cdevlib](https://github.com/jrowberg/i2cdevlib) installed<br>
<br>
Project was developed and tested on MultiWii Micro board. But should work on any Atmega328 board with BMA6050 I2C sensor.<br>
<br>
To measure vibrations you should follow this steps:<br>
<br>
1. Open sketch in Arduino
2. Flash it into the board
3. Attach board to bow with motor we testing with zip tie
4. Plug motor ESC to PIN9 of board
5. Power plug power battery to ESC
5. Plug USB cable to PC and open Arduino Port Monitor and connect it to COM port of your board
6. In Port Monitor, send any symbol to begin calibration process.
7. Board should be still while calibrating.
8. You will see something like this it Port Monitor window:
9. After calibration you can start measurements
10. Send PWM timing an number to board (from 1000 to 2000), or .(dot) to use 1200. btw. I've chosen this value as most effective for measurements. Keep in ming that greater values will make your copter fly. Hold it strongly while using geater values.
11. Vibrimeter will spin up motor, and measure vibrations (6 sec. by default)
12. After measurement you will see the results:
13. As for balancing tecnique, I use next algorithm:
  1. measure vibrations on bare motor with prop
  2. apply zip tie on motor and run one more test
  3. move zip tie at 1/3 or turnover and measure once more
  4. move zip tie again at 1/3 and measure last time
  5. now you have 4 veasurements to compare and chhose the best
  6. recheck best position and remove tie
  7. don't forget tie lock position
  8. apply little pierce of scotch to the place of lock
  9. remeasure
  10. keep going 8-9 while vibrations are reducing
  11. you have balanced motor!!
  
