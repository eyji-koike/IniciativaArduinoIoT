//
//
//
//

// includes
#include "Functions.h"

// declaration of variables
float lat, lon;                 // two float variables to store the latitude and logitude
SoftwareSerial gpsSerial(2, 3); // define the rx, tx for the GPS
TinyGPS gps;                    // Create a tiny gps object
int speed = 9600;               // baud rate or speed of our connections
unsigned long date, time, age;  // variable to store date time and age if any

// here we make the setup
void setup()
{
    Serial.begin(speed);    // Start the serial comm with the computer
    gpsSerial.begin(speed); // Star the communication between uno and the gps module
}

void loop()
{
    bool newData;
    for (unsigned long start = millis(); millis() - start < 1000;)
    {
        while (gpsSerial.available()) // check for gps data
        {
            char c = gpsSerial.read();
            Serial.write(c);
            if (gps.encode(c)) // Atribui true para newData caso novos dados sejam recebidos
                newData = true;
        }
    }
    if (newData)
    {
        float flat, flon;
        unsigned long age;
        gps.f_get_position(&flat, &flon, &age);
        Serial.print("\nLAT=");
        Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
        Serial.print(" LON=");
        Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
        Serial.print(" SAT=");
        Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
        Serial.print(" PREC=");
        Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
        Serial.println();
        Serial.println();
    }
    delay(5000); 
}