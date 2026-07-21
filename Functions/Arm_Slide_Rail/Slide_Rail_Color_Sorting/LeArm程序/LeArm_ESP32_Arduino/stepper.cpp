#include <Wire.h>
#include "stepper.hpp"


Stepper::Stepper()
{
  iic = nullptr;
}

void Stepper::init(IIC* iic_obj)
{
  iic = iic_obj;
}

void Stepper::reset(void)
{
  uint8_t Reposition = 1;
  iic->wireWriteDataArray(STEPPER_I2C_ADDR , MOTOR_AUTO_REPOSITION_ADDR, &Reposition, 1); //写1自动归位，归位后模块自动置0
  delay(50);
  while (Reposition == 1)
  {
    iic->wireReadDataArray(STEPPER_I2C_ADDR , MOTOR_AUTO_REPOSITION_ADDR, &Reposition, 1);
    delay(50);
  }
}

//设置细分模式(0,1,2,3,7  -->  1, 1/2, 1/4, 1/8, 1/16步)    默认0x07
void Stepper::driver_mode(uint8_t mode)
{
  iic->wireWriteDataArray(STEPPER_I2C_ADDR , STEPS_DRIVER_MODE_ADDR, &mode, 1);
}

// 设置步长，可正负
void Stepper::set_step(int32_t step)
{
  iic->wireWriteDataArray(STEPPER_I2C_ADDR , STEPS_ADDR, (uint8_t*)&step, 4);
}



void Stepper::set_step_time(uint16_t time)
{
  iic->wireWriteDataArray(STEPPER_I2C_ADDR , STEPS_TIME_ADDR,(uint8_t*)&time,2);
}
