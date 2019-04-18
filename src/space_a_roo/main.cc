#include <Arduino.h>
#include <Wire.h>
#include "wiring_private.h"

#include "spacestack.h"
#include "battery.hpp"

#define I2C_SDA_1 22 // PA12
#define I2C_SCL_1 38 // PA13

#define BAT_SNS_A_PIN 15 // PB08, Analog Pins
#define BAT_SNS_B_PIN 16 // PB09, Analog Pins

#define STRAT_TX_PIN 1 // PA10
#define STRAT_RX_PIN 4 // PA08

#define STRAT_BAUD 9600

Uart SerialStrato(&sercom0, STRAT_RX_PIN, STRAT_TX_PIN, SERCOM_RX_PAD_0, UART_TX_PAD_2);
TwoWire myWire(&sercom2, I2C_SDA_1, I2C_SCL_1);
Battery battery(BAT_SNS_A_PIN, BAT_SNS_B_PIN);

void requestEvent() {
  Serial.println("It beeped!");
  myWire.write("hello ");
}

void SERCOM0_Handler(void) {
  SerialStrato.IrqHandler();
}

void logBatInfo() {
  Battery::cell_voltage_t cells = battery.readVoltage();
  Serial.print("cell A voltage: ");
  Serial.println(cells.cellA, 4);
  Serial.print("cell B voltage: ");
  Serial.println(cells.cellB, 4);
  Serial.print("Total voltage: ");
  Serial.println(cells.total, 4);
}

void setup() {
  Serial.begin(9600);
  SerialStrato.begin(STRAT_BAUD);
  myWire.begin(spacestack_space_a_roo);
  pinPeripheral(I2C_SDA_1, PIO_SERCOM);
  pinPeripheral(I2C_SCL_1, PIO_SERCOM);

  // i think these aren't needed because already defined 
  // correctly in project's variant.cpp file
  pinPeripheral(STRAT_RX_PIN, PIO_SERCOM);
  pinPeripheral(STRAT_TX_PIN, PIO_SERCOM);
  pinPeripheral(BAT_SNS_A_PIN, PIO_ANALOG);
  pinPeripheral(BAT_SNS_B_PIN, PIO_ANALOG);

  myWire.onRequest(requestEvent);

}

void loop() {
  Serial.println("Boop!");

  if (SerialStrato.available()) {
    int altitude = SerialStrato.readStringUntil('\n').toInt();
    SerialStrato.flush();
    Serial.print("Alt: ");
    Serial.println(altitude);
    //Serial.print("hi");
  }
  logBatInfo();
  //delay(500);
}

extern "C" {
  void SERCOM2_Handler(void);
  void SERCOM2_HANDLER(void) {
    myWire.onService();
  }
}

// I2C MASTER COMMUNICATION, FOR USE WITH SPACEHOPPER

// #include <Arduino.h>
// #include <Wire.h>
// #include "wiring_private.h"
// 
// #include "spacestack.h"
// #include <TinyGPS++.h>
// 
// #define I2C_SDA_1 22 // PA12
// #define I2C_SCL_1 38 // PA13
// 
// TwoWire myWire(&sercom2, I2C_SDA_1, I2C_SCL_1);
// 
// void setup() {
  // Serial.begin(9600);
  // myWire.begin();
  // pinPeripheral(I2C_SDA_1, PIO_SERCOM);
  // pinPeripheral(I2C_SCL_1, PIO_SERCOM);
// }
// 
// void loop() {
  // Serial.println("Hello!");
  // myWire.requestFrom(spacestack_space_a_roo, 6);
  // while (myWire.available()) {
    // char c = myWire.read();
    // Serial.print(c);
  // }
  // delay(500);
// }
// 
// 
// 
// 
// extern "C" {
  // void SERCOM2_Handler(void);
  // void SERCOM2_HANDLER(void) {
    // myWire.onService();
  // }
// }
