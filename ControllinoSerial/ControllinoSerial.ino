/*
  Main sketch for serial communication between
  Hintpad and Controllino MINI/MAXI/MEGA

  gregor [at] pfeffermind.de

  -> Send serial data to the Controllino in order to switch pins/relays on and off
  -> Controllino reports HIGH and LOW reads from input pins to Hintpad via serial connection

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

String currentVersion = "1.1";

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
const int iocount1 = 8;
const int iocount2 = 6;
const int iocount3 = 8;
int outputD[] = {4, 5, 6, 7, 8, 9, A4, A5};
int outputR[] = {4, 5, 6, 7, 8, 9};
// inputs
int inputA[] = {CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2, CONTROLLINO_A3, CONTROLLINO_A4, 
                CONTROLLINO_A5, CONTROLLINO_IN0, CONTROLLINO_IN1};
                
int inputVal[8]; // input values
int oldInputVal[8]; // old input values for debouncing
boolean isSent[8]; // sent flags

#elif CMODL == 2
// MAXI
// outputs
const int iocount1 = 12;
const int iocount2 = 10;
const int iocount3 = 12;
int outputD[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int outputR[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
// inputs
int inputA[] = { CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2, CONTROLLINO_A3, CONTROLLINO_A4,
                 CONTROLLINO_A5, CONTROLLINO_A6, CONTROLLINO_A7, CONTROLLINO_A8, CONTROLLINO_A9, 
                 CONTROLLINO_IN0, CONTROLLINO_IN1 };

int inputVal[12]; // input values
int oldInputVal[12]; // old input values for debouncing
boolean isSent[12]; // sent flags
#elif CMODL == 3
// MEGA
// outputs
const int iocount1 = 20;
const int iocount2 = 16;
const int iocount3 = 21;
int outputD[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 42, 43, 44, 45, 46, 47, 48, 49};
int outputR[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
// inputs
//int inputA[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

int inputA[] = {CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2, CONTROLLINO_A3, CONTROLLINO_A4,
                              CONTROLLINO_A5, CONTROLLINO_A6, CONTROLLINO_A7, CONTROLLINO_A8, CONTROLLINO_A9,
                              CONTROLLINO_A10, CONTROLLINO_A11, CONTROLLINO_A12, CONTROLLINO_A13, CONTROLLINO_A14, CONTROLLINO_A15,
                              CONTROLLINO_I16, CONTROLLINO_I17, CONTROLLINO_I18, CONTROLLINO_IN0, CONTROLLINO_IN1
                             };

int inputVal[iocount3]; // input values
int oldInputVal[iocount3]; // old input values for debouncing
boolean isSent[iocount3]; // sent flags
#endif

boolean hasKeypad = false; // is there a (9 button) keypad on A1 - A9?
int keypadWinPin = 36;    // some pin that is set to HIGH when keypad game is won
int keypadInput[] =       {0, 0, 0, 0, 0, 0, 0}; // input array for 7 digits
int keypadSolution[] =    {9, 6, 8, 4, 1, 5, 7}; // the input array has to look like this

// button de-bounce 
int debounceMS [iocount3]; // stores the current millis()-values for each input
int doDebounce [iocount3]; // button input debounce flags
const int debounceTime = 75; // in ms

#include "swfunc.h"
#include "listenerfunc.h"

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port
  }

  // welcome message
  Serial.println("CONTROLLINO SERIAL READY!");
  Serial.println ("Ver. " + currentVersion);
  Serial.println ("Built on " __DATE__ " at " __TIME__);
  Serial.print ("Controllino model #"); Serial.println ( CMODL );
  Serial.print ("Digital Pins: "); Serial.println ( iocount1 );
  Serial.print ("Relay Pins: "); Serial.println ( iocount2 );
  Serial.print ("Input Pins: "); Serial.println ( iocount3 );

  for (int i = 0; i < iocount3; i++) {
    pinMode (inputA[i], INPUT);
  }
  for (int i = 0; i < iocount2; i++) {
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

