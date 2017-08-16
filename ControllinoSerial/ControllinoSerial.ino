/*
  Main sketch for serial communication between
  Hintpad and Controllino MINI/MAXI/MEGA

  gregor@pfeffermind.de

  -> Send serial data to the Controllino in order to switch pins/relays on and off
  -> Controllino reports HIGH and LOW reads from A0-AX via serial connection

  Controllino PinOut sheets: https://controllino.biz/downloads/

  usage:
  send integers enclosed by < and > chars via serial port 
  <pin, value>
  pin        the pin you want to switch
  value      0 = off, 1 = on

  NOTE: 
  on the MINI analog inputs A4 and A5 are analog only!
  In order to work with Hintpad the values have to be digital, 
  so A4 and A5 are mapped to digital values (see listenerfunc.h)
  
  Prints i/o info to serial monitor enclosed in square brackets:
  [pin, value]
*/

String currentVersion = "1.0";

#include <Controllino.h>

// ****************************** //
// set up Controllino model here:
// 1 = MINI
// 2 = MAXI
// 3 = MEGA
#define CMODL 1
// ****************************** //

#if CMODL == 1
// MINI
// outputs
int iocount1 = 8;
int iocount2 = 6;
int outputD[] = {4, 5, 6, 7, 8, 9, A4, A5};
int outputR[] = {4, 5, 6, 7, 8, 9};
// inputs
int inputA[] = {CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2, CONTROLLINO_A3, CONTROLLINO_A4, CONTROLLINO_A5};
int inputVal[6]; // input values
int oldInputVal[6]; // old input values for debouncing
boolean isSent[6]; // sent flags

#elif CMODL == 2
// MAXI
// outputs
int iocount1 = 12;
int iocount2 = 10;
int outputD[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int outputR[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
// inputs
int inputA[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
int inputVal[10]; // input values
int oldInputVal[10]; // old input values for debouncing
boolean isSent[10]; // sent flags

#elif CMODL == 3
// MEGA
// outputs
int iocount1 = 20;
int iocount2 = 16;
int outputD[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 42, 43, 44, 45, 46, 47, 48, 49};
int outputR[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
// inputs
int inputA[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};
int inputVal[16]; // input values
int oldInputVal[16]; // old input values for debouncing
boolean isSent[16]; // sent flags
#endif

#include "swfunc.h";
#include "listenerfunc.h";

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port
  }

  Serial.println("CONTROLLINO SERIAL READY!");
  Serial.println ("Ver. " + currentVersion);
  Serial.println ("Built on " __DATE__ " at " __TIME__);
  
  for (int i = 0; i < iocount2; i++) {
    pinMode (inputA[i], INPUT);
    pinMode (outputR[i], OUTPUT);
  }
  for (int j = 0; j < iocount1; j++) {
    pinMode (outputD[j], OUTPUT);
  }
}

void loop() {
  listenSerial(); // listen for serial message
  listenPins();
}

