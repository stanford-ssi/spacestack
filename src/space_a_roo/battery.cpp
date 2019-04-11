#include "battery.hpp"
#include <Arduino.h>

uint8_t bat_sns_a;
uint8_t bat_sns_b;

Battery::Battery(uint8_t bat_sns_a_pin, uint8_t bat_sns_b_pin)
{
    bat_sns_a = bat_sns_a_pin;
    bat_sns_b = bat_sns_b_pin;
}

Battery::cell_voltage_t Battery::readVoltage()
{
    uint16_t vSenseA, vSenseB;
    vSenseA = analogRead(bat_sns_a);
    vSenseB = analogRead(bat_sns_b);

    float vDivA = 0.6520; // 20 kohm: 10kohm voltage divider (~0.66)+ other factors
    float vDivB = 0.3256; // 10 kohm: 20kohm voltage divider (~0.33)+ other factors

    cell_voltage_t result;

    result.cellA = 3.3F * vSenseA / vDivA / 1024;
    result.total = 3.3F * vSenseB / vDivB / 1024;
    result.cellB = result.total - result.cellA;

    return result;
}