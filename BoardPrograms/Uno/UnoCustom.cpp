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


// this function was made to get GPS information
Telemetry getGPS(SoftwareSerial &comm, TinyGPS &gps)
{
    Telemetry telemetry;
    bool newData = false;
    while (comm.available()) // check for gps data
    {
        char c = comm.read();
        // Serial.write(c);         //uncomment this to se the raw output of gps info
        if (gps.encode(c))          // Atribui true para newData caso novos dados sejam recebidos
            newData = true;
    }
    if (newData)
    {
        gps.f_get_position(&telemetry.lat, &telemetry.lon, &telemetry.fixAge); // parse lat, long and fix age into our struct
        gps.get_datetime(&telemetry.date, &telemetry.time, &telemetry.fixAge); // parse date, time and fix age into our struct
        telemetry.alt = gps.f_altitude();                                      // parse altitude into our struct
        telemetry.course = gps.f_course();                                     // parse the course into our struct
        telemetry.speedkmph = gps.f_speed_kmph();                              // parse the speed into our struct
        telemetry.numSat = gps.satellites();                                   // parse the current number of sattelites that we are connected to
        telemetry.hdop = gps.hdop();                                           // parse the quality of signal
        Serial.println(telemetry.hdop);
    }
    return telemetry;
}

// this function was made to update the LCD within our request with a static message
void updateLCD(LiquidCrystal &lcd, unsigned long globalEntranceCounter, unsigned long globalExitCounter)
{
    String messageFirstLine = "En: ";
    String messageSecondLine = "Ex(S): ";
    lcd.clear();                                         // clear the display
    lcd.setCursor(0, 0);                                 // set the cursor to the first column first row
    lcd.print(messageFirstLine + globalEntranceCounter); // print message and the counter value
    lcd.setCursor(0, 1);                                 // go to the first column of the second line
    lcd.print(messageSecondLine + globalExitCounter);    // print message and the counter value
}


