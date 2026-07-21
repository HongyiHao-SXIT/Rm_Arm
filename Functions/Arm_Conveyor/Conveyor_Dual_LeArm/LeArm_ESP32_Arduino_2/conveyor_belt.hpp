#ifndef CONVEYOR_BELT_H
#define CONVEYOR_BELT_H

#include <Arduino.h>
#include "IIC.hpp"

#define CONVEYOR_BELT_I2C_ADDR        0x35

class CONVEYOR_BELT{
  private:
    IIC* iic;
  public:
    CONVEYOR_BELT();
    void init(IIC* iic_obj);
    void set_speed(int8_t speed);
};

#endif //CONVEYOR_BELT_H
