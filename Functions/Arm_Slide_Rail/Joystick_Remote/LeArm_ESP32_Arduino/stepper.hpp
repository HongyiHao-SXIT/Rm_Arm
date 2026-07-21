
#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>
#include "IIC.hpp"

#define STEPPER_I2C_ADDR        0x35

#define MOTOR_STATE_ADDR                    20
// EN,SLEEP,RST  --> bit2,1,0   默认0x03

#define STEPS_DRIVER_MODE_ADDR              21
// MS3,2,1  --> bit2,1,0
//(0,1,2,3,7  -->  1, 1/2, 1/4, 1/8, 1/16步)    默认0x07

#define MOTOR_AUTO_REPOSITION_ADDR          22
//写1自动归位（可读可写），归位后模块自动置0

#define STEPS_ADDR                          24
//相对步数，int32型数据（可读可写）写入数值开始运动。
//写入数值大于0向前运动，每运动一步自减1，直到减为0停止。
//写入数值小于0向后运动，每运动一步自增1，直到增为0停止。

#define STEPS_TIME_ADDR                     28
//每步多长时间，uint16型数据（单位毫秒） 默认1毫秒

class Stepper{
  private:
    IIC* iic;
  public:
    Stepper();
    void init(IIC* iic_obj);
    void reset(void);
    //设置细分模式(0,1,2,3,7  -->  1, 1/2, 1/4, 1/8, 1/16步)    默认0x07
    void driver_mode(uint8_t mode);
    // 设置步长，可正负
    void set_step(int32_t step);
    // 设置每步走的时间，默认1ms
    void set_step_time(uint16_t time);
};

#endif //STEPPER_H
