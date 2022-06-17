#include "CustomFunctions.h"
// Example 5 - Receive with start- and end-markers combined with parsing
#include "SerialTransfer.h"


SerialTransfer myTransfer;
SoftwareSerial Serial1(2,3);

struct STRUCT {
  char z;
  float y;
} testStruct;

char arr[6];


void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  myTransfer.begin(Serial1);
}


void loop()
{
  if(myTransfer.available())
  {
    // use this variable to keep track of how many
    // bytes we've processed from the receive buffer
    uint16_t recSize = 0;

    recSize = myTransfer.rxObj(testStruct, recSize);
    Serial.print(testStruct.z);
    Serial.print(testStruct.y);
    Serial.print(" | ");

    recSize = myTransfer.rxObj(arr, recSize);
    Serial.println(arr);
  }
}
