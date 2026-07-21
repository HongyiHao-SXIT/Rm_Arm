#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "WonderMV.hpp"

Led_t led_obj;
Buzzer_t buzzer_obj;
Button_t key_obj;
LeArm_t arm;

IIC iic;
WonderMV mv;

MV_RESULT_ST result;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源
  
  arm.init();
  led_obj.init(IO_LED);
  buzzer_obj.init(IO_BUZZER);
  iic.init();
  mv.init(&iic);

  Serial.begin(9600);
  buzzer_obj.blink(1500, 100, 100, 1);
}

enum
{
	RECOGNITION = 0,
	FIND_TAG_POSITION,
	RESET_ARM
};
uint8_t tag_id;
uint8_t fsm_state = RECOGNITION;
float position[6][3] = {{15.0f,  15.0f, 15.0f},
						{15.0f,   0.0f, 15.0f},		/* 复位位置 */
					    {15.0f, -15.0f, 15.0f},
					    {15.0f,  15.0f, -5.0f},		/* 左侧放置位置 */
					    {15.0f,   0.0f, -5.0f},		/* 中间放置位置 */
					    {15.0f, -15.0f, -5.0f}};	/* 右侧放置位置 */


void loop() {
  switch(fsm_state)
	{
	  case RECOGNITION:{
      mv.get_result(TAG_REG , &result);
		  delay(20);
		  if(result.id != 0)
		  {
        delay(200);
        mv.get_result(TAG_REG , &result);
        if(result.id != 0)
        {
          tag_id = result.id;
          buzzer_obj.blink(1500, 100, 100, 1);
          arm.claw_set(30, 0);
          delay(500);
          fsm_state = FIND_TAG_POSITION;
        }
		  }
    }break;

	  case FIND_TAG_POSITION:{
		  for(uint8_t i = 0; i< 3; i++)
		  {
			  arm.coordinate_set(position[3 + i][0], position[3 + i][1], position[3 + i][2], 0, -90, 90, 600);
			  delay(5000);
			  mv.get_result(TAG_REG , &result);
			  if(result.id == tag_id)
			  {
          buzzer_obj.blink(1500, 100, 100, 1);
          arm.claw_set(90, 0);
          delay(500);
          arm.coordinate_set(position[i][0], position[i][1], position[i][2], 0, -90, 90, 600);
          delay(600);
          break;
			  }
		  }
		  fsm_state = RESET_ARM;
    }

	  case RESET_ARM:{
		  arm.coordinate_set(position[1][0], position[1][1], position[1][2], 0, -90, 90, 600);
		  delay(1000);
		  fsm_state = RECOGNITION;
    }break;
		 
	}
}
