#include <TinyGPS++.h>
#include <Arduino.h>
#include "wiring_private.h"
//#include <SoftwareSerial.h>

void displayInfo();

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
#define PIN_RX_GPS 30
#define PIN_TX_GPS 19
#define PIN_RX_S6C 1
#define PIN_TX_S6C 4
#define GPS_BAUD 9600

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
Uart SerialGPS(&sercom5, PIN_RX_GPS, PIN_TX_GPS, SERCOM_RX_PAD_2, UART_TX_PAD_0);
Uart SerialS6C(&sercom0, PIN_RX_GPS, PIN_TX_GPS, SERCOM_RX_PAD_2, UART_TX_PAD_0);

void SERCOM5_Handler(void) {
  SerialGPS.IrqHandler();
}

void SERCOM0_Handler(void) {
  SerialS6C.IrqHandler();
}

void setup()
{
  pinPeripheral(PIN_RX_GPS, PIO_SERCOM_ALT);
  pinPeripheral(PIN_TX_GPS, PIO_SERCOM_ALT);

  pinPeripheral(PIN_RX_S6C, PIO_SERCOM_ALT);
  pinPeripheral(PIN_TX_S6C, PIO_SERCOM_ALT);

  Serial.begin(115200);
  SerialGPS.begin(GPS_BAUD);
  SerialS6C.begin(9600);
  
  delay(10000);
  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  if (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read())) {
      displayInfo();
      delay(50);
    }
  }

  if (SerialS6C.available() > 0) {
    Serial.write(SerialS6C.read());
  }
    

  // if (millis() > 5000 && gps.charsProcessed() < 10)
  // {
  //   Serial.println(F("No GPS detected: check wiring."));
  //   while(true);
  // }
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
