
//includes
#include "GSMFunctions.h"

//variables

//serial
unsigned int baud = 9600;  //this is the communications baud rate
SoftwareSerial simModem(10,11); //(GSM Tx ---> UNO Rx = 10 || GSM Rx ---> UNO Tx = 11)
//network
const char apn[] = "timbrasil.br"; //this is your provider's apn
const char user[] = "tim";  //this is the apn's user
const char pass[] = "tim";  //this is the apn's password
//mqtt
//add variables in the future
//create objects
TinyGsm modem(simModem); //a tinygsm object made with our simModem
TinyGsmClient client(modem); //a tinygsm client made out of our modem object
//PubSubClient mqtt(client); //a pubsub client made out of our tinygsm client

//setup routine
void setup()
{
  Serial.begin(baud);
  simModem.begin(baud);
  
  Serial.println("System start.");
  modem.restart();
  delay(3000);
  simModem.println("AT+CPIN?\n");
  Serial.print(simModem.read());
  Serial.println("Modem: " + modem.getModemInfo());
  Serial.println("Searching for telco provider.");
  if(!modem.waitForNetwork())
  {
    Serial.println("fail");
    while(true);
  }
  Serial.println("Connected to telco.");
  Serial.println("Signal Quality: " + String(modem.getSignalQuality()));

  Serial.println("Connecting to GPRS network.");
  if (!modem.gprsConnect(apn, user, pass))
  {
    Serial.println("fail");
    while(true);
  }
  Serial.println("Connected to GPRS: " + String(apn));
}

void loop(){
    if(Serial.available())
  {
    delay(10);
    Serial.println("The IP address is " + modem.getLocalIP());
    
  }
}