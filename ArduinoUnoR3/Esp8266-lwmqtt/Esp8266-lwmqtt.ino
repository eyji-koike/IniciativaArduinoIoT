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
  Serial.begin(115200);
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

telemetry tel;
SerialTransfer transfer;
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(57600);
  setupCloudIoT(); // Creates globals for MQTT
  pinMode(LED_BUILTIN, OUTPUT);
  transfer.begin(Serial);
}

static unsigned long lastMillis = 0;
void loop()
{
  if (!mqtt->loop())
  {
    mqtt->mqttConnect();
  }

  delay(10); // <- fixes some issues with WiFi stability
  if (transfer.available()){
    uint16_t recSize =0;

    recSize = transfer.rxObj(tel, recSize);
 
  }
  // TODO: Replace with your code here
  if (millis() - lastMillis > 60000) {
    lastMillis = millis();

    String payload =
      String("{\"Latitude\":") + tel.lat +
      String(",\"Longitude\":")+ tel.lon +
      String(",\"HDOP\":")+ tel.hdop +
      String(",\"Altitude\":")+ tel.alt +
      String(",\"Course\":")+ tel.course +
      String(",\"Speed\":")+ tel.speedkmph +
      String(",\"NumSat\":")+ tel.numSat +
      String(",\"Date\":")+ tel.date +
      String(",\"Time\":")+ tel.time + 
      String(",\"fixAge\":")+ tel.fixAge +
      String(",\"Entrance\":")+ tel.entrance +
      String(",\"Exit\":")+ tel.exit +
      String("}");
    publishTelemetry(payload);
  }
  
}
#endif
