Motor Vibrometer
================

Project is intended to help in dynamic balancing of copter motors with props.<br>
There are huge of ways to measure vibrations produced by motor. Begining from sencing it by hand, ending laser method. I suggest to use flight controller features to measure vibrations and show it in clear numbers for comparison.<br>
Project was developed and tested on MultiWii Micro board. But should work on any Atmega328 board with BMA6050 I2C sensor.<br>
You may use flight controller installed on your copter. But since they are (in most cases) damped, measurements will be less indicative. I advice to use separate flight controller or arduino board with BMA6050 sensor.<br>
Project requires [i2cdevlib](https://github.com/jrowberg/i2cdevlib) installed<br>
<h2>HowTo</h2>
To measure vibrations you should follow this steps:<br>
<br>
1. Open sketch in Arduino<br>
2. Flash it into the board<br>
3. Attach board to bow with motor we testing with zip tie<br>
4. Plug motor ESC to PIN9 of board<br>
5. Plug power battery to ESC<br>
5. Plug USB cable to PC and open Arduino Port Monitor and connect it to COM port of your board<br>
6. Check that all cables and components are safe from prop blades. <br>
7. In Port Monitor, send any symbol to begin calibration process.<br>
8. Board should be still while calibrating.<br>
9. You will see something like this it Port Monitor window:<br>
<pre>
Initializing I2C devices...
Testing device connections...
MPU6050 connection successful

Calibrating.............
Calibrations:
-1251	-1109	1132	10	23	-11

Send motor PWM number or .(dot) to start measurement:
</pre>
10. After calibration you can start measurements<br>
11. Send PWM timing an number to board (from 1000 to 2000), or .(dot) to use 1200. btw. I've chosen this value as most effective for measurements. Keep in ming that greater values will make your copter fly and you vill have extra vibrations in z-axis. I've found that it most convinient to put copter on table and make measured bow, with motor, ouside the edge of the table. <br>
12. Vibrometer will spin up motor, and measure vibrations (6 sec. by default)<br>
13. After measurement you will see the results:<br>
<pre>
Starting motor at: 1200
Spin up...
Measuring 5 seconds...
...........
	ax	ay	az	gx	gy	gz
Max:	228	264	424	52	50	46	
Avg:	58	106	101	11	11	10	
Measurements count: 2275
</pre>
14. As for balancing tecnique, I use next algorithm:<br>
  1. measure vibrations on bare motor with prop
  2. apply zip tie on motor and run one more test
  3. move zip tie at 1/3 or turnover and measure once more
  4. move zip tie again at 1/3 and measure last time
  5. now you have 4 measurements to compare and choose the best
  6. recheck best position and remove tie
  7. don't forget tie lock position
  8. apply little pierce of metallic adhesive tape to the place of lock
  9. remeasure
  10. keep going 8-9 while vibrations are reducing
  11. you have balanced motor!!
  
