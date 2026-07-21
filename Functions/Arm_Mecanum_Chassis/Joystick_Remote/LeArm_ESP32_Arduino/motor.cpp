#include "motor.hpp"

Motor::Motor(){

}

void Motor::init(IIC* iic_obj){
    iic = iic_obj;
    MotorType = MOTOR_TYPE_TT;  //设置电机类型
    MotorEncoderPolarity = 1;   //电机极性控制变量
}

//在电机类型地址中写入电机类型编号
void Motor::set_type(uint8_t type){
    MotorType = type;
    iic->wireWriteDataArray(CAM_DEFAULT_I2C_ADDRESS,MOTOR_TYPE_ADDR,&MotorType,1);//在电机类型地址中写入电机类型编号
}

//设置电机极性设置
void Motor::set_polarity(uint8_t Polarity){
    MotorEncoderPolarity = Polarity;
    iic->wireWriteDataArray(CAM_DEFAULT_I2C_ADDRESS,MOTOR_ENCODER_POLARITY_ADDR,&MotorEncoderPolarity,1);//设置电机极性设置
}

//读取电机电压 mV
uint16_t Motor::read_bat(void){
    uint16_t v;
    iic->wireReadDataArray(CAM_DEFAULT_I2C_ADDRESS,ADC_BAT_ADDR,data,2);
    v = data[0] + (data[1]<<8); //转换电压
    return v;
}

//读取4个编码电机各自的总脉冲值 参数：uint32_t [4]
void Motor::read_pulse_count(uint32_t *list){
    iic->wireReadDataArray(CAM_DEFAULT_I2C_ADDRESS,MOTOR_ENCODER_TOTAL_ADDR,(uint8_t*)list,16);
}

void Motor::set_speed(int8_t *speed_list){
    iic->wireWriteDataArray(CAM_DEFAULT_I2C_ADDRESS ,MOTOR_FIXED_SPEED_ADDR,(uint8_t*)speed_list,4); //控制电机转动
}
