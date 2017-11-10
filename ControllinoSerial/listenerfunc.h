/*
   Serial and pin listener functions
   debounces button/trigger inputs
*/

#include <Arduino.h>
boolean newData = false;
int sArduPin, sPinValue, sPinDuration;  // stores incoming integers from serial connection
char receivedChars[32];                 // stores the whole message coming from Hintpad
int inMessage [] = { 0 , 0 , 0 };       // stores single values of a message coming from Hintpad
String sendStrg;
int btnCounter = 0;

void showParsedData() {
  if (receivedChars[0] != 0) {
    Serial.print("[");
    Serial.print(inMessage[0]); Serial.print(","); Serial.print(inMessage[1]); Serial.print(","); Serial.print(inMessage[2]);
    Serial.println("]");
    newData = false;
  }
}

void parseData (int *theMessage) {
  // split the data into its parts using strtok
  char * strtokIndx; // this is used by strtok() as an index
  strtokIndx = strtok(receivedChars, ",");  // get the first part
  String tmpStrtok = String(strtokIndx);
  if (tmpStrtok.indexOf("A") < 0) {
    sArduPin = atoi (strtokIndx);     // convert to int
  } else {
    // on the MINI, digital outputs D6 and D7 are mapped to A4 and A5
    // i.e. if you want to turn on D6 you have to digitalWrite (A4, HIGH);
    // Hinpad already sends A4,A5 to switch D6,D7 but we cannot convert string to constant name
    // so we need to re-map the string here:
    (tmpStrtok == "A4") ? sArduPin = A4 : sArduPin = A5;
  }
  strtokIndx = strtok(NULL, ",");   // continue...
  sPinValue = atoi(strtokIndx);
  strtokIndx = strtok(NULL, ",");
  sPinDuration = atoi(strtokIndx);
  newData = false;
  theMessage[0] = sArduPin;
  theMessage[1] = sPinValue;
  theMessage[2] = sPinDuration;
}

void clearData () {
  for ( int i = 0; i < sizeof(receivedChars);  ++i )
    receivedChars[i] = (char)0;
}

void listenSerial() {
  static boolean recvInProgress = false; // used to ignore inputs without start and end markers
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= 32) { // prevent overflow
          ndx = 31;
        }
      } else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
        parseData (&inMessage[0]);

        // find output-pin in arrays
        int outputPin = inMessage[0];
        int outputType = 0; // 0=None[default], 1=Digital, 2=Relay
        int newValue = inMessage[1];
        int posInArray;

        // try to find the pin in the relay pin list
        for (int i = 0; i < iocount2; i++) {
          if (outputR[i] == outputPin) {
            outputType = 2; // it's a relay!
            posInArray = i;
          }
        }

        // also go through outputD even if we already have found a relay
        // ~~> on the MINI, D and R pins are the same
        for (int j = 0; j < iocount1; j++) {
          if (outputD[j] == outputPin) {
            outputType = 1; // it's a digital pin!
            posInArray = j;
          }
        }
        turnDR ( outputPin, newValue, outputType, posInArray ); // see swfunc.h
        clearData (); // we're done
      }
    } else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void listenPins () {
  // listen for inputs on A0 - AX and send event string via serial
  for (int i = 0; i < iocount3; i++) {
    String serString;
    inputVal[i] = digitalRead(inputA[i]);

    // ANALOG INPUT MAPPING
    // when using MINI, read A4 and A5 (analog only!) again
    // and convert them to digital HIGH/LOW values, using a threshold
    if (CMODL == 1) {
      if  (i >= 4 && i <= 5) {
        int aInput  = analogRead (inputA[i]) ; // temporarily store the analog input value
        int aThreshold = 700; // the HIGH/LOW treshold of the analog pin
        if (aInput >= aThreshold) {
          inputVal[i] = HIGH;
        } else {
          inputVal[i] = LOW;
        }
      }
    }

    // debounce inputs
    if (inputVal[i] == HIGH ) {
      if (doDebounce[i] == false && isSent[i] == false) {
        doDebounce[i] = true;
        debounceMS[i] = millis();
      }
      if ( isSent[i] == false &&  millis() - debounceMS[i] >= debounceTime && doDebounce[i] == true) {
        debounceMS[i] = millis();
        doDebounce[i] = false;
        isSent[i] = true;

        serString = "[A"; // default
        serString += i;   // default

        // MINI
        if (CMODL == 1) {
          if (i == 6) {
            serString = "[IN0";
          }
          if (i == 7) {
            serString = "[IN1";
          }
        }

        // MAXI
        if (CMODL == 2) {
          if (i == 10) {
            serString = "[IN0";
          }
          if (i == 11) {
            serString = "[IN1";
          }
        }

        // MEGA
        if (CMODL == 3) {
          if (i >= 16 && i <= 18) {
            serString = "[I";
            serString += i;
          }
          if (i == 19) {
            serString = "[IN0";
          }
          if (i == 20) {
            serString = "[IN1";
          }
        }

        serString += ",1]";
        Serial.println(serString); // REPORT via serial
        btnCounter += 1;

      }
    } else if (inputVal[i] == LOW && isSent[i] == true) {
      if (doDebounce[i] == false) {
        debounceMS[i] = millis();
        doDebounce[i] = true;
      }
      if (millis() - debounceMS[i] >= debounceTime) {
        isSent[i] = false;
        debounceMS[i] = millis();
        doDebounce[i] = false;

        serString = "[A"; // default
        serString += i;   // default

        // MINI
        if (CMODL == 1) {
          if (i == 6) {
            serString = "[IN0";
          }
          if (i == 7) {
            serString = "[IN1";
          }
        }

        // MAXI
        if (CMODL == 2) {
          if (i == 10) {
            serString = "[IN0";
          }
          if (i == 11) {
            serString = "[IN1";
          }
        }

        // MEGA
        if (CMODL == 3) {
          if (i >= 16 && i <= 18) {
            serString = "[I";
            serString += i;
          }
          if (i == 19) {
            serString = "[IN0";
          }
          if (i == 20) {
            serString = "[IN1";
          }
        }

        serString += ",0]";
        Serial.println(serString); // REPORT via serial
      }
    }  else {
      debounceMS[i] = millis();
      doDebounce[i] = false;
    }
  }
}
