/*********************************************************************
 *  Copyright 2022 IniciativaArduinoIoT                               *
 *  Licensed under the Apache License, Version 2.0 (the "License");   *
 *  you may not use this file except in compliance with the License.  *
 *  You may obtain a copy of the License at                           *
 *                                                                    *
 *  http://www.apache.org/licenses/LICENSE-2.0                        *
 *                                                                    *
 *  Unless required by applicable law or agreed to in writing,        *
 *  software distributed under the License is distributed on an       *
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,      *
 *  either express or implied. See the License for the specific       *
 *  language governing permissions and limitations under the License. *
 **********************************************************************/

// Includes
#include "UnoCustom.h"

// Global definitions and variables
// definition of pins
const int rs = 13,
          en = 12,
          d4 = 11,
          d5 = 10,
          d6 = 9,
          d7 = 8,
          gpsRX = 6, // gps TX ==> Digital pin (D6)
    gpsTX = 7,       // gps RX ==> Digital pin (D7)
    buttonEntrance = 5,
          buttonExit = 4,
          espRX = 2,   // esp TX ==> Digital pin (D2)
    espTX = 3,         // gps RX ==> Digital pin (D3)
    updateTime = 1000; // time between gps updates

unsigned long frontCounter;
unsigned long backCounter;
static long startTime;
bool firstRun;                // save the first setup condition
const int serialBaud = 57600; // the speed of our connections
// create our objects and struct
Telemetry newTelemetry;                    // our telemetry struct
SoftwareSerial gpsSerial(gpsRX, gpsTX);    // define the ports to communicate with gps
SoftwareSerial esp1(espRX, espTX);         // define the ports to communicate with esp
TinyGPS gps;                               // our tinygps object gps
SerialTransfer transferTel;                // our transferTel of type SerialTransfer
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // our luqid crystal

// setup run
void setup()
{
    firstRun = true;                 // flag ou first run
    pinMode(buttonEntrance, INPUT);  // set as input to read our button
    pinMode(buttonExit, INPUT);      // set as input to read our button
    esp1.begin(serialBaud);          // start serial with esp01
    Serial.begin(serialBaud);        // start serial with computer
    gpsSerial.begin(serialBaud);     // start serial with gps
    transferTel.begin(esp1);         // start our transfer protocol
    lcd.begin(16, 2);                // setup the correct info about our lcd scree, 16char and 2 lines
    Serial.print("Setup completed"); // Print it back to user
    startTime = millis();            // get the initial time
}

// main loop
void loop()
{
    unsigned long currentMillis = millis(); // get the current time
    newTelemetry = getGPS(gpsSerial, gps);  // get the gps info
    Serial.print("Got gps\n");
    while (firstRun = false || currentMillis - startTime < updateTime)
    {
        // if we are on the first run or its time to update the gps we do it and get the time
        firstRun = false;
        startTime = currentMillis;                              // parse the current time for our control
        bool frontButton = readSwitchDebounced(buttonEntrance); // read the button that represents the front
        bool backButton = readSwitchDebounced(buttonExit);      // read the button that represents the back
        // if the front button was pressed, set it in the struct, send, and increase the counter
        if (frontButton == true || backButton == true)
        {
            if (frontButton == true && backButton == false)
            {
                newTelemetry.entrance = 1;
                frontCounter++;
                Serial.print("Should have increased front counter\n");
            }
            else if (frontButton == false && backButton == true)
            {
                newTelemetry.exit = 1;
                backCounter++;
                Serial.print("Should have increased back counter\n");
            }
            else
            {
                newTelemetry.entrance = 1;
                newTelemetry.exit = 1;
                frontCounter++;
                backCounter++;
                Serial.print("Should have increased both");
            }
            // use this variable to keep track of how many
            // bytes we're stuffing in the transmit buffer
            uint16_t sendSize = 0;
            ///////////////////////////////////////// Stuff buffer with struct
            sendSize = transferTel.txObj(newTelemetry, sendSize);
            ///////////////////////////////////////// Send buffer
            transferTel.sendData(sendSize);
            updateLCD(lcd, frontCounter, backCounter); // refresh our display
        }
    }
}