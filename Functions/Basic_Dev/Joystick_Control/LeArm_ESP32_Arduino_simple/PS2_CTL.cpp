#include "PS2_ctl.hpp"
/* 该文件一般放在src/PS2文件夹中 */
// #include "./../../Robot_arm.hpp"
// #include "./../../Hiwonder.hpp"
// #include "./../../Config.h"
#include "Robot_arm.hpp"
#include "Hiwonder.hpp"
#include "Config.h"

TaskHandle_t PS2TaskHandle;

float map_x, map_y, map_z;
#define Serial_PS2 Serial2

static float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return out_min + (x - in_min) * ((out_max - out_min) / (in_max - in_min));
}

void PS2_CTL::init(void)
{
  Serial_PS2.begin(9600 ,SERIAL_8N1 , USB_TX , -1);
  action_running_time = 1000;
}

void PS2_CTL::PS2_Task(LeArm_t* robot,Led_t* led,Buzzer_t* buzzer)
{
  static uint8_t first_flag = 1;
  if(first_flag!=0)
  {
    first_flag = 0;
    if(mode_count == PS2_SINGLE_SERVO_MODE)
    {
      led->blink(250,250,0);
    }else{
      led->blink(250,0,0);
    }
  }
  receive_msg();
  get_result(robot,led,buzzer);
  delay(20);
}

void PS2_CTL::clear_rec(void)
{
    while (Serial_PS2.available()>0)
    {
        Serial_PS2.read();
    }
}

void PS2_CTL::receive_msg(void)
{
  static uint8_t step = 0;
  static uint8_t index = 0;
  while (Serial_PS2.available()>0) {
    switch(step){
      case 0:{
        if(Serial_PS2.read() == FRAME_HEADER)
        {
          step++;
        }
      }break;
      case 1:{
        if(Serial_PS2.read() == FRAME_HEADER)
        {
          index = 0;
          step++;
        }else{
          step = 0;
        }
      }break;
      case 2:{
        recbuff[index++] = Serial_PS2.read();
        if(index > 9)
        {
          rec_flag = true;
          step = 0;
        }
      }break;
      default:{
        step = 0;
      }break;
    }

    if(rec_flag)
    {
      if(recbuff[9] == 0x80 && recbuff[6] == 0x80 &&
          recbuff[7] == 0x80 && recbuff[8] == 0x80)
      {
        keyvalue.mode = PS2_SINGLE_SERVO_MODE;
      }
      else if(recbuff[9] == 0x7F && recbuff[6] == 0x7F &&
              recbuff[7] == 0x7F && recbuff[8] == 0x7F)
      {
        keyvalue.mode = PS2_COORDINATE_MODE;
      }

      switch(keyvalue.mode){
        case PS2_SINGLE_SERVO_MODE:
          keyvalue.bit_triangle = 	recbuff[3] & 0xFF;
          keyvalue.bit_circle = 	(recbuff[3] & 0xFF) >> 1;
          keyvalue.bit_cross = 		(recbuff[3] & 0xFF) >> 2;
          keyvalue.bit_square = 	(recbuff[3] & 0xFF) >> 3;
          keyvalue.bit_l1 =  		(recbuff[3] & 0xFF) >> 4;
          keyvalue.bit_r1 =  		(recbuff[3] & 0xFF) >> 5;
          keyvalue.bit_l2 =  		(recbuff[3] & 0xFF) >> 6;
          keyvalue.bit_r2 =  		(recbuff[3] & 0xFF) >> 7;
          keyvalue.bit_select = 	recbuff[4] & 0xFF;
          keyvalue.bit_start = 		(recbuff[4] & 0xFF) >> 1;
          keyvalue.bit_leftjoystick_press =  (recbuff[4] & 0xFF) >> 2;
          keyvalue.bit_rightjoystick_press = (recbuff[4] & 0xFF) >> 3;
          keyvalue.bit_left = recbuff[6] == 0x00 ? 1 : 0;
          keyvalue.bit_right = recbuff[6] == 0xFF ? 1 : 0;
          keyvalue.bit_up = recbuff[7] == 0x00 ? 1 : 0;
          keyvalue.bit_down = recbuff[7] == 0xFF ? 1 : 0;
          break;
        
        case PS2_COORDINATE_MODE:
          keyvalue.bit_triangle = 	recbuff[3] & 0xFF;
          keyvalue.bit_circle = 	(recbuff[3] & 0xFF) >> 1;
          keyvalue.bit_cross = 		(recbuff[3] & 0xFF) >> 2;
          keyvalue.bit_square = 	(recbuff[3] & 0xFF) >> 3;
          keyvalue.bit_l1 =  		(recbuff[3] & 0xFF) >> 4;
          keyvalue.bit_r1 =  		(recbuff[3] & 0xFF) >> 5;
          keyvalue.bit_l2 =  		(recbuff[3] & 0xFF) >> 6;
          keyvalue.bit_r2 =  		(recbuff[3] & 0xFF) >> 7;
          keyvalue.bit_select = 	recbuff[4] & 0xFF;
          keyvalue.bit_start = 		(recbuff[4] & 0xFF) >> 1;
          keyvalue.bit_leftjoystick_press =  (recbuff[4] & 0xFF) >> 2;
          keyvalue.bit_rightjoystick_press = (recbuff[4] & 0xFF) >> 3;
          keyvalue.left_joystick_x = recbuff[6];
          keyvalue.left_joystick_y = recbuff[7];
          keyvalue.right_joystick_x = recbuff[8];
          keyvalue.right_joystick_y = recbuff[9];
          switch(recbuff[5])
          {
            case 0x00:
              keyvalue.bit_up = 1;
              break;
            
            case 0x01:
              keyvalue.bit_up = 1;
              keyvalue.bit_right = 1;
              break;
            
            case 0x02:
              keyvalue.bit_right = 1;
              break;
            
            case 0x03:
              keyvalue.bit_down = 1;
              keyvalue.bit_right = 1;
              break;
            
            case 0x04:
              keyvalue.bit_down = 1;
              break;
            
            case 0x05:
              keyvalue.bit_down = 1;
              keyvalue.bit_left = 1;
              break;
            
            case 0x06:
              keyvalue.bit_left = 1;
              break;
            
            case 0x07:
              keyvalue.bit_up = 1;
              keyvalue.bit_left = 1;
              break;
            
            case 0x0F:
              keyvalue.bit_up = 0;
              keyvalue.bit_down = 0;
              keyvalue.bit_left = 0;
              keyvalue.bit_right = 0;
              break;
          }
          break;
          
        default:
          break;
      }
      break;
    }
  }
}

/* 用户可在这里添加自己的手柄处理逻辑 */
int PS2_CTL::get_result(LeArm_t* robot,Led_t* led,Buzzer_t* buzzer)
{
  if(rec_flag){
    rec_flag = false;
    if(keyvalue.mode == PS2_SINGLE_SERVO_MODE)
    {
      if(keyvalue.bit_triangle == 1)
      {
        robot->knot_run(1, 1600, action_running_time);
      }
      else if(keyvalue.bit_cross == 1)
      {
        robot->knot_run(1, MIN_DUTY, action_running_time);
      }
      else if(keyvalue.bit_triangle == 0 || keyvalue.bit_cross == 0)
      {
        robot->knot_stop(1);
      }
    }
  }
  return 0;
}

