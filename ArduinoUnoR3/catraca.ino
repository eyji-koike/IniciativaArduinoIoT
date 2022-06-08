/* Projeto Curto Circuito – Módulo ESP01 - ESP8266 Modo AT */

#include <SoftwareSerial.h>
#include <Arduino.h>

using namespace std;
SoftwareSerial wiFiBoard(2,3,false);  /* pino 2 TX , pino 3 RX */


void setup(){
Serial.begin(115200);         /* Opções para ajuste 9600, 19200, 57600 e */
wiFiBoard.begin(115200);        /* 115200 dBs */
}
void loop(){
if(wiFiBoard.available())       /* Confere se a comunicação está acessível */
{
while(wiFiBoard.available()) {  /* Enquanto estiver acessível */
char c =wiFiBoard.read();      /* Le o caractere. */
Serial.write(c);              /* Escreve no monitor serial */
}
}
if(Serial.available()){ 
delay(1000); 
String command="";
while(Serial.available()) {   /* Le o comando */
command+=(char)Serial.read();
}
wiFiBoard.println(command);    /* Envia o comando para o ESP */
}
}