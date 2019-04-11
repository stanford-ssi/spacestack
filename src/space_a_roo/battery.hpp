#pragma once

#include <inttypes.h>
class Battery
{
  public:

    struct cell_voltage_t
    {
        float cellA; //this is the lower cell
        float cellB; //this is the upper cell
        float total; //both cells
    };
    
    Battery(uint8_t bat_sns_a_pin, uint8_t bat_sns_b_pin);
    cell_voltage_t readVoltage();
};