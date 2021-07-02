Control of Servo and Stepper Motors using Touchscreen display
=============================================================

The project consists on building a circuit with the Arduino Pro Mini to control a stepper motor and a RC servo motor using an LCD display / touchscreen as user interface.

An LCD touch screen will be used to enter a position command in degrees that the motors will rotate.  This will be an integer between 0 and 360 for the stepper, and 0 to 90 for the RC servo.  A rotation command will be prompted on the LCD display, and the number entered should be echoed at the LCD.

- On start, prompt the user to select which motor to drive (servo  or stepper)  
- Direction should be prompted as CW/CCW  
- After holding position for “n” seconds, the motor returns to the zero position.  
- Program returns to the start prompt



**Hardware used:**
- Elegoo Uno R3
- Elegoo 2.8 Inches TFT Touch Screen with SD Card Socket
- Servo  Motor
- Stepper Motor 17hs16-2004s1
- TB6600 Stepper motor driver
