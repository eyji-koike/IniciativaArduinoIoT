







/* This is a header file that points to other header files used by our functions and to the functions file*/
#pragma once
#include <stdio.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <time.h>
#include <MQTT.h>
//#include <CloudIoTCore.h>
//#include <CloudIoTCoreMqtt.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>


using namespace std;
//Start new objects 
Client *netClient;
//CloudIoTCoreDevice *device;
//CloudIoTCoreMqtt *mqtt;
MQTTClient *mqttClient;
unsigned long iss = 0;
String jwt;

/* This is a helper function used inside connectToWiFiNetwork */
void clearESP_buffer(unsigned int timeout);

/* This is a helper function used inside connectToWiFiNetwork */
boolean waitForResponse(String target1,  unsigned int timeout);

/* This functions connects the board to wifi */
void connectToWiFiNetwork();

//this function is meant to reconnect to the wifi if ever needed
void reconnect();




