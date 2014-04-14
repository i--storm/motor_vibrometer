Motor Vibrometer
================

Measures vibrations of copter motor<br>
Project requres [i2cdevlib](https://github.com/jrowberg/i2cdevlib) installed<br>
<br>
Project was developed and tested on MultiWii Micro board. But should work on any Atmega328 board with BMA6050 I2C sensor.<br>
<br>
To measure vibrations you should follow this steps:<br>
<br>
1. Open sketch in Arduino<br>
2. Flash it into the board<br>
3. Attach board to bow with motor we testing with zip tie<br>
4. Plug motor ESC to PIN9 of board<br>
5. Power plug power battery to ESC<br>
5. Plug USB cable to PC and open Arduino Port Monitor and connect it to COM port of your board<br>
6. Check that all cables and components are safe from prop blades. <br>
7. In Port Monitor, send any symbol to begin calibration process.<br>
8. Board should be still while calibrating.<br>
9. You will see something like this it Port Monitor window:<br>
10. After calibration you can start measurements<br>
11. Send PWM timing an number to board (from 1000 to 2000), or .(dot) to use 1200. btw. I've chosen this value as most effective for measurements. Keep in ming that greater values will make your copter fly and you vill have extra vibrations in z-axis. I've found that it most convinient to put copter on table and make measured bow, with motor, ouside the edge of the table. <br>
12. Vibrometer will spin up motor, and measure vibrations (6 sec. by default)<br>
13. After measurement you will see the results:<br>
14. As for balancing tecnique, I use next algorithm:<br>
  1. measure vibrations on bare motor with prop
  2. apply zip tie on motor and run one more test
  3. move zip tie at 1/3 or turnover and measure once more
  4. move zip tie again at 1/3 and measure last time
  5. now you have 4 measurements to compare and chhose the best
  6. recheck best position and remove tie
  7. don't forget tie lock position
  8. apply little pierce of scotch to the place of lock
  9. remeasure
  10. keep going 8-9 while vibrations are reducing
  11. you have balanced motor!!
  
