





// This file contains all the functions used to connect our board using the Uno and the ESP01
// We only need to include our header, the other functions are the alredy
#include "functions.h"

//this is for us to use String instead of std::String
using namespace std;

//define the ports connected to the wifi module UART
SoftwareSerial wifiModule(2,3,false);       //object wifiModule of class SoftwareSerial

//here we hardcode our network information - this is not ideal
//const char* ssid = "WIFI NETWORK";          // WIFI network SSID or AP SSID
// ßconst char* password = "PASSWORD";          // WIFI network PASSWORD

boolean debug=true;                         //Activate debug mode to show messages back

/* This is a helper function used inside connectToWiFiNetwork */
void clearESP_buffer(unsigned int timeout){
    unsigned long t=millis();
    char c;
    while (t+timeout>millis()){
      if (wifiModule.available()){
        c=wifiModule.read();
        if (debug) Serial.print(c);
      }
    }
};

/* This is a helper function used inside connectToWiFiNetwork */
boolean waitForResponse(String target1,  unsigned int timeout){
    String data;
    char a;
    unsigned long start = millis();
    boolean rValue=false;
    while (millis() - start < timeout) {
        while(wifiModule.available() > 0) {
            a = wifiModule.read();
            if (debug) Serial.print(a);
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            rValue=true;
            break;
        } 
    }
    return rValue;
};

/* This functions connects the board to wifi */
void connectToWiFiNetwork(){
    Serial.println("Connecting to "+String(ssid));
    clearESP_buffer(1000);
    wifiModule.println("AT+GMR");
    delay(100);
    waitForResponse("OK",1000);
    
    wifiModule.println("AT+CWMODE=1");  // configure as client
    waitForResponse("OK",1000);
    
    //--- connect
    wifiModule.print("AT+CWJAP=\"");
    wifiModule.print(ssid);
    wifiModule.print("\",\"");
    wifiModule.print(password);
    wifiModule.println("\"");
    waitForResponse("OK",10000);

    wifiModule.println("AT+CIFSR");           // get ip address
    waitForResponse("OK",1000);
    
};
