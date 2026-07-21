#include <Wire.h>
#include "conveyor_belt.hpp"


CONVEYOR_BELT::CONVEYOR_BELT()
{
  iic = nullptr;
}

void CONVEYOR_BELT::init(IIC* iic_obj)
{
  iic = iic_obj;
  iic->wireWriteByte(CONVEYOR_BELT_I2C_ADDR, 0);
}

void CONVEYOR_BELT::set_speed(int8_t speed)
{
  iic->wireWriteByte(CONVEYOR_BELT_I2C_ADDR, speed);
}
