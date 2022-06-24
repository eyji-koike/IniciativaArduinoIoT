/******************************************************************************
 * Copyright 2018 Google
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#if defined(ARDUINO_SAMD_MKR1000) or defined(ESP32)
#define __SKIP_ESP8266__
#endif

#if defined(ESP8266)
#define __ESP8266_MQTT__
#endif

#ifdef __SKIP_ESP8266__

#include <Arduino.h>

void setup(){
  Serial.begin(9600);
}

void loop(){
  Serial.println("Hello World");
}

#endif

#ifdef __ESP8266_MQTT__
#include <CloudIoTCore.h>
#include "esp8266_mqtt.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

using namespace std;
// here we create our objects
Telemetry telemetry;      // Our telemetry object
SerialTransfer transfer;  // Our transfer object
bool readyToSend;         // flag if there is things to send

// this is our callback that does stuff when there is a message available
void hi(){
  uint16_t recSize = 0;
  recSize = transfer.rxObj(telemetry, recSize);
  readyToSend = true;
    if (readyToSend == true) {
    String payload =
      String("{\"Latitude\":") + telemetry.lat +
      String(",\"Longitude\":")+ telemetry.lon +
      String(",\"HDOP\":")+ telemetry.hdop +
      String(",\"Altitude\":")+ telemetry.alt +
      String(",\"Course\":")+ telemetry.course +
      String(",\"Speed\":")+ telemetry.speedkmph +
      String(",\"NumSat\":")+ telemetry.numSat +
      String(",\"Date\":")+ telemetry.date +
      String(",\"Time\":")+ telemetry.time + 
      String(",\"fixAge\":")+ telemetry.fixAge +
      String(",\"Entrance\":")+ telemetry.entrance +
      String(",\"Exit\":")+ telemetry.exit +
      String("}");
    publishTelemetry(payload);
    readyToSend = false;
    }
}


// this is a constant required for allocation
const functionPtr callbackArr[] = { hi };

// here we do our setup
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);            // hopefully here we are starting our transfer
  setupCloudIoT();                // Creates globals for MQTT
  pinMode(LED_BUILTIN, OUTPUT);   // blinks the esp led

  // here we create our config object to pass to transfer
  configST myConfig;
  myConfig.debug = true;
  myConfig.callbacks = callbackArr;
  myConfig.callbacksLen = sizeof(callbackArr) / sizeof(functionPtr);
  
  // here we start our transfer with the callback
  transfer.begin(Serial, myConfig);
}

// main loop
void loop()
{
  transfer.tick(); // poll the transfer to see if there is anything to send
  // Check if we need to reconnect to MQTT
  if (!mqtt->loop())
  {
    mqtt->mqttConnect();
  }
  delay(10); // <- fixes some issues with WiFi stability 
}
#endif
