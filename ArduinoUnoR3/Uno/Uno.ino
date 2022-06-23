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
          gpsRX = 6,         // gps TX ==> Digital pin (D6)
          gpsTX = 7,         // gps RX ==> Digital pin (D7)
          buttonEntrance = 5,
          buttonExit = 4,
          espRX = 2,         // esp TX ==> Digital pin (D2)
          espTX = 3,         // gps RX ==> Digital pin (D3)
          updateTime = 2000; // time between gps updates
unsigned long frontCounter;  // front global counter
unsigned long backCounter;   // back global counter
static long startTime;       // timer
const int serialBaud = 57600; // the speed of our connections
///////////////////////////////////////////// create our objects and struct
Telemetry newTelemetry;                    // our telemetry struct
SoftwareSerial gpsSerial(gpsRX, gpsTX);    // define the ports to communicate with gps
SoftwareSerial esp1(espRX, espTX);         // define the ports to communicate with esp01
TinyGPS gps;                               // our tinygps object gps
SerialTransfer transferTel;                // our transferTel of type SerialTransfer
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // our liquid crystal lcd
DailyStruggleButton entrance;              // our daily struggle entrance button object
DailyStruggleButton exitB;                 // our daily struggle exit button object

// setup run
void setup()
{
    lcd.begin(16, 2);                // setup the correct info about our lcd scree, 16char and 2 lines
    lcd.setCursor(0,0);
    lcd.print("Arduino IoT");
    lcd.setCursor(0,1);
    lcd.print("Press to start");
    esp1.begin(serialBaud);          // start serial with esp01
    Serial.begin(serialBaud);        // start serial with computer
    gpsSerial.begin(serialBaud);     // start serial with gps
    transferTel.begin(esp1);         // start our transfer protocol
    startTime = millis();            // get the initial time
    newTelemetry = getGPS(gpsSerial, gps);          // start our struct
    entrance.set(buttonEntrance, callbackEntrance); // set our button entrance
    exitB.set(buttonExit, callbackExit);            // set our button exit
    Serial.println("Setup completed");              // Print it back to user
}

// main loop
void loop()
{   
    unsigned long currentCountFront = frontCounter; // take the current count value and save it somewhere else
    unsigned long currentCountBack = backCounter;   // take the current count value and save it somewhere else
    entrance.poll();                                // poll the entrance button (from the DailyStruggleButton library)
    exitB.poll();                                   // poll the exit button 
    unsigned long currentMillis = millis();         // get the current time
    // check if it's time to update the GPS information
    if (currentMillis - startTime > updateTime){
      newTelemetry = getGPS(gpsSerial, gps);        // get the gps info
      // Serial.println("Got gps");                 // this was made to see if this was executing (the gps delay on cold boot killed me)
      Serial.print(newTelemetry.lon);               // this was made to see if this was executing (the gps delay on cold boot killed me)      
      Serial.print("|");                            // this was made to see if this was executing (the gps delay on cold boot killed me)
      Serial.println(newTelemetry.lat);             // this was made to see if this was executing (the gps delay on cold boot killed me) 
      startTime = currentMillis;                    // define the current time as the time we start
    }
    // if the front button was pressed, set it in the struct, send, and increase the counter
    if (currentCountFront != frontCounter || currentCountBack != backCounter)
    {
      if (currentCountFront != frontCounter){
        newTelemetry.entrance = 1;
        newTelemetry.exit = 0;
      }
      else if ( currentCountBack != backCounter){
        newTelemetry.exit = 1;
        newTelemetry.entrance =0;
      }
      updateLCD(lcd, frontCounter, backCounter); // refresh our display
      // use this variable to keep track of how many
      // bytes we're stuffing in the transmit buffer
      Serial.println(newTelemetry.exit);        // this was made to see if this was executing (the gps delay on cold boot killed me)
      Serial.println(newTelemetry.entrance);    // this was made to see if this was executing (the gps delay on cold boot killed me)
      Serial.println(newTelemetry.course);      // this was made to see if this was executing (the gps delay on cold boot killed me)
      Serial.println(newTelemetry.alt);         // this was made to see if this was executing (the gps delay on cold boot killed me)
      uint16_t sendSize = 0;
      ///////////////////////////////////////// Stuff buffer with struct
      sendSize = transferTel.txObj(newTelemetry, sendSize);
      ///////////////////////////////////////// Send buffer
      transferTel.sendData(sendSize);
    }
}
     
// Entrance callback function
void callbackEntrance(byte buttonEvent)
{
    switch (buttonEvent)
    {
    case onRelease:
        frontCounter++;
        Serial.println("Front");
        break;
    }
}

//Exit callback function
void callbackExit(byte buttonEvent)
{
    switch (buttonEvent)
    {
    case onRelease:
        backCounter++;
        Serial.println("Back");
        break;
    }
}