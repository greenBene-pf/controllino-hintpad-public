# controllino-serial
general Controllino MINI/MAXI/MEGA sketch for Hintpad!-input/output via serial com port

## Setup
- Install Arduino IDE https://www.arduino.cc/en/main/software 
- or Atom with PlatformIO plugin http://docs.platformio.org/en/latest/ide/atom.html
- Search and install Controllino board in Board Manager 
- connect Controllino and set up board type and serial port in Board Manager
- open controllino-serial sketch
- set up Controllino model in Sketch (1 = MINI, 2 = MAXI, 3 = MEGA) - example: #define CMODL 2
- compile and upload

## Update
In this common Controllino sketch I had to remove the timer functions, because the ATmega chip has only 5 timer interrupts. So I decided that it's better to use the Hintpad that allows us to have ALL the pins switched by timers.
