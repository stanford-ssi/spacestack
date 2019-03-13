#include <Arduino.h>
#include <Wire.h>
#include "wiring_private.h"

#include "spacestack.h"

#define I2C_SDA_1 22 // PA12
#define I2C_SCL_1 38 // PA13

TwoWire myWire(&sercom2, I2C_SDA_1, I2C_SCL_1);

void requestEvent() {
  Serial.println("It beeped!");
  myWire.write("hello ");
}

void setup() {
  Serial.begin(9600);
  myWire.begin(spacestack_space_a_roo);
  pinPeripheral(I2C_SDA_1, PIO_SERCOM);
  pinPeripheral(I2C_SCL_1, PIO_SERCOM);
  myWire.onRequest(requestEvent);
}

void loop() {
  Serial.println("Boop!");
  delay(500);
}

extern "C" {
  void SERCOM2_Handler(void);
  void SERCOM2_HANDLER(void) {
    myWire.onService();
  }
}
