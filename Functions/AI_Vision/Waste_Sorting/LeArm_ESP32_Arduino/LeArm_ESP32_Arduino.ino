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

enum
{
	RESET_ARM = 0,
	RECOGNITION,
	PLACE,
};

uint8_t ao1_state;
uint8_t ao2_state;
uint8_t left_state;
uint8_t right_state;
uint8_t fsm_state = RESET_ARM;
float position[8][3] = {{15.0f,   0.0f, 15.0f},		/* 复位位置 */
						{12.0f,  12.0f, -5.0f},		/* 左侧识别位置 */
					    {15.0f,  15.0f, -8.0f},		/* 左侧夹取位置 */
						{12.0f, -12.0f, -5.0f},		/* 右侧识别位置 */
						{15.0f, -15.0f, -8.0f},		/* 右侧夹取位置 */
					    {20.0f,   0.0f, -5.0f},		/* 中间放置位置 */
						{15.0f,  15.0f, 15.0f},		/* 左侧复位位置 */
			  			{15.0f, -15.0f, 15.0f}};	/* 右侧复位位置 */	

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

  pinMode(PA5, INPUT);
  pinMode(IO13, INPUT);

  arm.action_run(0,1);
  delay(3000);
}



void loop() {
  switch(fsm_state)
	{
	  case RESET_ARM:{
      if(digitalRead(PA5) == LOW) //夹取左边到中间
      {
        left_state = 1;
        fsm_state = RECOGNITION;
        arm.coordinate_set(position[1][0], position[1][1], position[1][2], 0, -90, 90, 1000);
        delay(1100);
      }else if(digitalRead(IO13) == LOW) //夹取右边到中间
      {
        right_state = 1;
        fsm_state = RECOGNITION;
			  arm.coordinate_set(position[3][0], position[3][1], position[3][2], 0, -90, 90, 1000);
        delay(1100);
      }
      delay(50);
    }break;
	  
	  case RECOGNITION:{
		  delay(5000);
      mv.get_result(OBJECT_REG , &result);
		  if(result.id != 1)
		  {
			  left_state = 0;
			  right_state = 0;
			  fsm_state = RESET_ARM;
			  arm.coordinate_set(position[0][0], position[0][1], position[0][2], 0, -90, 90, 1000);
			  delay(2000);
			  break;
		  }else{
			  if(left_state == 1)
			  {
				  left_state = 0;
				  arm.coordinate_set(position[2][0], position[2][1], position[2][2], 0, -90, 90, 600);
				  delay(800);
				  arm.claw_set(30, 0);
				  delay(300);
				  arm.coordinate_set(position[0][0], position[0][1], position[0][2], 0, -90, 90, 1000);
				  delay(800);
			  }
			  else if(right_state == 1)
			  {
				  right_state = 0;
				  arm.coordinate_set(position[4][0], position[4][1], position[4][2], 0, -90, 90, 600);
				  delay(800);
				  arm.claw_set(30, 0);
				  delay(300);
				  arm.coordinate_set(position[0][0], position[0][1], position[0][2], 0, -90, 90, 1000);
				  delay(800);
			  }
			  fsm_state = PLACE;
		  }
    }break;
 
	  case PLACE:
		  arm.coordinate_set(position[5][0], position[5][1], position[5][2], 0, -90, 90, 600);
		  delay(800);
		  arm.claw_set(90, 0);
		  delay(800);
		  arm.coordinate_set(position[0][0], position[0][1], position[0][2], 0, -90, 90, 1000);
      delay(1500);
		  fsm_state = RESET_ARM;
		  break;
	}
  delay(50);
}

