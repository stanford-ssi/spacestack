#include <Arduino.h>
#include <Wire.h>
#include "wiring_private.h"

#include "spacestack.h"
#include <TinyGPS++.h>

#define I2C_SDA_1 22 // PA12
#define I2C_SCL_1 38 // PA13

TwoWire myWire(&sercom2, I2C_SDA_1, I2C_SCL_1);

void setup() {
  Serial.begin(9600);
  myWire.begin();
  pinPeripheral(I2C_SDA_1, PIO_SERCOM);
  pinPeripheral(I2C_SCL_1, PIO_SERCOM);
}

void loop() {
  Serial.println("Hello!");
  myWire.requestFrom(spacestack_space_a_roo, 6);
  while (myWire.available()) {
    char c = myWire.read();
    Serial.print(c);
  }
  delay(500);
}




extern "C" {
  void SERCOM2_Handler(void);
  void SERCOM2_HANDLER(void) {
    myWire.onService();
  }
}
