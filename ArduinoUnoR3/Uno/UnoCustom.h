// /********************************************************************* 
// *  Copyright 2022 IniciativaArduinoIoT                               *
// *  Licensed under the Apache License, Version 2.0 (the "License");   *
// *  you may not use this file except in compliance with the License.  *
// *  You may obtain a copy of the License at                           *
// *                                                                    *
// *  http://www.apache.org/licenses/LICENSE-2.0                        *
// *                                                                    *
// *  Unless required by applicable law or agreed to in writing,        *
// *  software distributed under the License is distributed on an       *
// *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,      *
// *  either express or implied. See the License for the specific       *
// *  language governing permissions and limitations under the License. *
// **********************************************************************/

//Includes
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TinyGPS.h>
#include <LiquidCrystal.h>
#include "SerialTransfer.h"

//define the structure of our telemetry
struct telemetry{
    long entrance;
    long exit;
    long hdop;
    float lat;
    float lon;
    float alt;
    float course;
    float speedkmph;
    unsigned short numSat;
    unsigned long date;
    unsigned long time;
    unsigned long fixAge;

};

// This function is used to get a debounced input from button
bool readSwitchDebounced(int buttonPin);

//this functions was made to update the LCD within our request
void updateLCD(LiquidCrystal &lcd, unsigned long globalEntranceCounter, unsigned long globalExitCounter);

//this function was made to simplify the data transfer
void sendTelemetry(telemetry &telemetry, SerialTransfer &transfer);

// this function was made to get GPS information
void getGPS(SoftwareSerial &comm, TinyGPS &gps, telemetry &telemetry);