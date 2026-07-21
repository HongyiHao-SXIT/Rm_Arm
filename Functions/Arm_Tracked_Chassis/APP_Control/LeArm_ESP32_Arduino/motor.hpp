#ifndef _MOTOR_HPP_
#define _MOTOR_HPP_

#include <Arduino.h>
#include "IIC.hpp"

#define CAM_DEFAULT_I2C_ADDRESS       (0x34)			//I2C地址
#define MOTOR_TYPE_ADDR               0x14 				//编码电机类型设置寄存器地址
#define MOTOR_FIXED_SPEED_ADDR       	0x33			//速度寄存器地址。属于闭环控制
#define MOTOR_ENCODER_POLARITY_ADDR   0x15				//电机编码方向极性地址
#define MOTOR_FIXED_PWM_ADDR      		0x1F			//固定PWM控制地址，属于开环控制
#define MOTOR_ENCODER_TOTAL_ADDR  		0x3C			//4个编码电机各自的总脉冲值
#define ADC_BAT_ADDR                  0x00				//电压地址


//电机类型具体地址
#define MOTOR_TYPE_WITHOUT_ENCODER        0 		//无编码器的电机,磁环每转是44个脉冲减速比:90  默认
#define MOTOR_TYPE_TT                     1 		//TT编码电机
#define MOTOR_TYPE_N20                    2 		//N20编码电机
#define MOTOR_TYPE_JGB37_520_12V_110RPM   3 		//磁环每转是44个脉冲   减速比:90  默认

class Motor{
  private:
    IIC* iic;
    uint8_t data[3];    //用于暂存电压ADC数据
    uint8_t MotorEncoderPolarity;   //电机极性控制变量
    uint32_t EncodeTotal[4];            //用于暂存电机累积转动量的值，正转递增，反转递减
    uint8_t MotorType;  //设置电机类型
  public:
    Motor();
    void init(IIC* iic_obj);
    void set_type(uint8_t type);
    void set_polarity(uint8_t Polarity);
    uint16_t read_bat(void);
    void read_pulse_count(uint32_t *list);
    void set_speed(int8_t *speed_list);
};


#endif //_MOTOR_HPP_
