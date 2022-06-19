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
telemetry tel;                             // our telemetry struct
SoftwareSerial gpsSerial(gpsRX, gpsTX);    // define the ports to communicate with gps
SoftwareSerial esp1(espRX, espTX);         // define the ports to communicate with esp
TinyGPS gps;                               // our tinygps object gps
SerialTransfer transferTel;                // our transferTel of type SerialTransfer
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // our luqid crystal

// setup run
void setup()
{
    firstRun = true;                 // flag ou first run
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
    // if we are on the first run or its time to update the gps we do it and get the time
    if (firstRun == true || currentMillis - startTime >= updateTime)
    {
        getGPS(gpsSerial, gps, tel); // get the gps info
        startTime = currentMillis;   // parse the current time for our control
    }
    bool frontButton = readSwitchDebounced(buttonEntrance); // read the button that represents the front
    bool backButton = readSwitchDebounced(buttonExit);      // read the button that represents the back
    // if the front button was pressed, set it in the struct, send, and increase the counter
    if (frontButton = true)
    {
        tel.entrance = 1;
        sendTelemetry(tel, transferTel);
        frontCounter++;
    }
    // if the back button was pressed, set it in the struct, send, and increase the counter
    if (backButton == true)
    {
        tel.exit = 1;
        sendTelemetry(tel, transferTel);
        backCounter++;
    }
    updateLCD(lcd, frontCounter, backCounter); // refresh our display
    firstRun = false;                          // set the first run as false
}