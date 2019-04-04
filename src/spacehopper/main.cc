/* Code for the RF carrier board at the top of the spacestack. Interfaces
 * with the GPS and S6C. May eventually use I2C to communicate with other
 * other boards in the stack.
 */
#include <TinyGPS++.h>
#include <Arduino.h>
#include "wiring_private.h"

#define PIN_RX_GPS 30
#define PIN_TX_GPS 19
#define GPS_BAUD 9600

#define PIN_RX_S6C 1
#define PIN_TX_S6C 4
#define S6C_BAUD 9600

#define GPS_DATA_INTERVAL 500 // send data every 500 milliseconds

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
Uart SerialGPS(&sercom5, PIN_RX_GPS, PIN_TX_GPS, SERCOM_RX_PAD_2, UART_TX_PAD_0);
Uart SerialS6C(&sercom0, PIN_RX_S6C, PIN_TX_S6C, SERCOM_RX_PAD_2, UART_TX_PAD_0);

void displayInfo();

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
  SerialS6C.begin(S6C_BAUD);
  delay(2000); // wait for serial monitor to be opened

  Serial.print(F("TinyGPS++ library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  if (SerialGPS.available() > 0) {
    bool time_to_send = lastSend - millis() > GPS_DATA_INTERVAL;
    if (time_to_send && gps.encode(SerialGPS.read())) {
      sendGPS();
      displayInfo();
    }
  }

  // if (SerialS6C.available() > 0) {
    // Serial.write(SerialS6C.read());
  // }
    

  // if (millis() > 5000 && gps.charsProcessed() < 10)
  // {
  //   Serial.println(F("No GPS detected: check wiring."));
  //   while(true);
  // }
}

void sendGPS() {
  /* Send GPS data to the S6C as comma-separated values.
   *
   * Format: lat, lon, hour, min, sec
   */
  if (gps.location.isValid()) {
    SerialS6C.print(gps.location.lat(), 6);
    SerialS6C.print(F(","));
    SerialS6C.print(gps.location.lng(), 6);
    SerialS6C.print(F(","));
  } else {
    SerialS6C.print(F"0,0,");
  }
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) SerialS6C.print(F("0"));
    SerialS6C.print(gps.time.hour());
    SerialS6C.print(F(","));
    if (gps.time.minute() < 10) SerialS6C.print(F("0"));
    SerialS6C.print(gps.time.minute());
    SerialS6C.print(F(","));
    if (gps.time.second() < 10) SerialS6C.print(F("0"));
    SerialS6C.print(gps.time.second());
  } else {
    SerialS6C.print(F"0,0,0");
  }
  SerialS6C.println();
}


void displayInfo() {
  /* Print out GPS data to the serial monitor.
   */
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
