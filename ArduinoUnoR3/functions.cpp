





// This file contains all the functions used to connect our board using the Uno and the ESP01
// We only need to include our header, the other functions are the alredy
#include "functions.h"
#include "ciotconfig.h"

//this is for us to use String instead of std::String
using namespace std;

//define the ports connected to the wifi module UART
SoftwareSerial wifiModule(6,7,false);       //object wifiModule of class SoftwareSerial
boolean debug=true;                         //Activate debug mode to show messages back to UI
int status = WL_IDLE_STATUS;                //Temporary flag attributed when wifi is started and stays active
//Until the number of trials expire (result in wl_no_shield or until a conecction is estabelished
//result (result in wl_connected)
 

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
boolean waitForResponse(String target1,  unsigned int timeout){ //this is meant as a buffer before sending another message
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
    WiFi.init(&wifiModule); //Inicia o modulo wifi
    //INÍCIO - VERIFICA SE O ESP8266 ESTÁ CONECTADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER
    if(WiFi.status() == WL_NO_SHIELD){
        while (true);
    }
    while(status != WL_CONNECTED){
     status = WiFi.begin(ssid, password);
    }
    
    
    // Serial.println("Connecting to "+String(ssid)); //output that is connecting to wifi
    // clearESP_buffer(1000);
    // wifiModule.println("AT+GMR");       //check version
    // waitForResponse("OK",1000);
    
    // wifiModule.println("AT+CWMODE=1");  // configure as client
    // waitForResponse("OK",1000);
    
    // //--- connect
    // wifiModule.print("AT+CWJAP=\""); 
    // wifiModule.print(ssid);
    // wifiModule.print("\",\"");
    // wifiModule.print(password);
    // wifiModule.println("\"");
    // waitForResponse("OK",10000);

    // wifiModule.println("AT+CIFSR");           // get ip address
    // waitForResponse("OK",1000);
    
};

//this function is meant to reconnect to the wifi if ever needed
void reconnect(){
    clearESP_buffer(1000);
    wifiModule.println("AT+CWQAP");     //disconnect
    waitForResponse("OK",1000);
    connectToWiFiNetwork(); //connect
};

