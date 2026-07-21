#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "WonderMV.hpp"
#include "ASR_module.hpp"
#include "Ultrasound.hpp"

Led_t led_obj;
Buzzer_t buzzer_obj;
Button_t key_obj;
LeArm_t arm;

IIC iic;
WonderMV mv;
ASR_MOUDLE asr;
Ultrasound ult;

float position[10][3] = {
            {15.0f,  15.0f, 15.0f},
						{15.0f,   0.0f, 15.0f},		/* 复位位置 */
            {15.0f, -15.0f, 15.0f},

            {15.0f,  15.0f, -3.0f},		/* 左侧放置位置 */
            {15.0f,   0.0f, -3.0f},		/* 中间放置位置 */
            {15.0f, -15.0f, -3.0f},	/* 右侧放置位置 */

            {15.0f,  15.0f, -6.0f},		/* 左侧夹取位置 */
            {15.0f,   0.0f, -6.0f},		/* 中间夹取位置 */
            {15.0f, -15.0f, -6.0f},	/* 右侧夹取位置 */

            {28.0f, 0.0f, 15.0f}	/* 递给用户 */
            };

MV_RESULT_ST result;
void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为高电平时，断开蓝牙模块电源
  
  arm.init();
  led_obj.init(IO_LED);
  buzzer_obj.init(IO_BUZZER);

  iic.init();
  mv.init(&iic);
  asr.init(&iic);
  ult.init(&iic);

  Serial.begin(9600);
  buzzer_obj.blink(1500, 100, 100, 1);
  ult.Color(0,0,0,0,0,0);
  arm.coordinate_set(position[1][0],position[1][1],position[1][2],0,-90,90,1000); //复位
  delay(3000);
}

uint8_t asr_result = 0;
uint8_t color_flag = 0;
uint8_t step = 0;

void loop() {
  switch(step)
  {
    case 0:{
      asr_result = asr.rec_recognition();
      if(asr_result == 0x69) //分拣红色
      {
        color_flag = 1;
        step = 1;
      }else if(asr_result == 0x6A){ //分拣绿色
        color_flag = 2;
        step = 1;
      }else if(asr_result == 0x6B){ //分拣蓝色
        color_flag = 3;
        step = 1;
      }
    }break;
    case 1:{
      int successful_flag = 0;
      while(1){
        for(int i = 0; i< 3; i++)
        {
          arm.coordinate_set(position[3 + i][0], position[3 + i][1], position[3 + i][2], 0, -90, 90, 600);
          delay(3000);
          mv.get_result(COLOR_REG , &result);
          if(result.id == color_flag)
          {
            buzzer_obj.blink(1500, 100, 100, 1);
            arm.coordinate_set(position[6 + i][0], position[6 + i][1], position[6 + i][2], 0, -90, 90, 500);
            delay(700);
            arm.claw_set(30, 0);
            delay(500);
            arm.coordinate_set(position[i][0], position[i][1], position[i][2], 0, -90, 90, 600); //起来
            delay(1000);
            arm.coordinate_set(position[9][0], position[9][1], position[9][2], 0, -90, 90, 600); //递给用户
            delay(1000);
            arm.claw_set(90, 0); //松开爪子
            delay(500);
            arm.coordinate_set(position[1][0],position[1][1],position[1][2],0,-90,90,1000); //复位
            delay(1000);
            successful_flag = 1;
            color_flag = 0;
            step = 0;
            break;
          }
        }
        if(successful_flag == 1)
        {
          break;
        }
      }
    }break;
  }
}

