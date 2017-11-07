/*
   switch functions for controllino
*/

#include <Arduino.h>

// switch digital pins and relays
void turnDR ( int thePin, int newValue, int outputType, int posInArray) {
  String serString; // string that will be sent via serial
  // compose serial string
  if (outputType == 1) {
    serString = "[D";
    serString += posInArray;
    serString += ",";
    serString += newValue;
    serString += "]";
    digitalWrite(outputD[posInArray], newValue); // switch the pin
  } else if (outputType == 2) {
    serString = "[R";
    serString += posInArray;
    serString += ",";
    serString += newValue;
    serString += "]";
    digitalWrite(outputR[posInArray], newValue); // switch the pin
  } else {
    serString = "[ERROR]";
  }
  Serial.println(serString); // REPORT via serial
}

