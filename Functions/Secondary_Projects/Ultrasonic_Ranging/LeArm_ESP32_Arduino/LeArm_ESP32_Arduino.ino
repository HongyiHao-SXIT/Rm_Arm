#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "Ultrasound.hpp"

LeArm_t arm;
Buzzer_t buzzer;

IIC iic;
Ultrasound ult;

#define ULTRASOUND_OFFSET 9.0f


void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为高电平时，断开蓝牙模块电源

  arm.init();

  iic.init();
  ult.init(&iic);
  Serial.begin(9600);
  buzzer.init(IO_BUZZER);

  arm.coordinate_set(15.0f, 0, 15, 0, -90, 90, 1000);
  delay(2000);
}


uint8_t running_state = 0;
uint8_t finish_count  = 0;
void loop() {
  uint16_t dis = ult.GetDistance();
  switch(running_state)
  {
    case 0:{
      if(dis >= 50 && dis <= 100)
			{
        delay(1500);
        dis = ult.GetDistance();
        if(dis >= 50 && dis <= 100)
			  {
          buzzer.blink(1500 , 100 , 100 , 1);
          arm.coordinate_set((float)(dis / 10) + ULTRASOUND_OFFSET, 0, -7, 0, -90, 90, 1000);
          while (1)
          {
            for(uint8_t i = 3; i < 7; i++)
            {
              if(arm.knot_finish(i))
              {
                finish_count++;
              }
            }
            if(finish_count == 4)
            {
              finish_count = 0;
              running_state = 1;
              break;
            }
            finish_count = 0;
            delay(100);
          }
        }
      }
    }break;
    case 1:{
      arm.claw_set(30, 500);
			delay(700);
			running_state = 2;
    }break;
    case 2:{
      arm.coordinate_set(15.0f, 0, 15, 0, -90, 90, 1000);
			while(1)
			{
				for(uint8_t i = 3; i < 7; i++)
				{
					if(arm.knot_finish(i))
					{
						finish_count++;
					}
				}
				if(finish_count == 4)
				{
					finish_count = 0;
					running_state = 3;
					break;
				}
				finish_count = 0;					
			}
    }break;
    case 3:{
      arm.coordinate_set(25.0f, 0, -5, 0, -90, 90, 1000); // + ULTRASOUND_OFFSET
			while(1)
			{
				for(uint8_t i = 3; i < 7; i++)
				{
					if(arm.knot_finish(i))
					{
						finish_count++;
					}
				}
				if(finish_count == 4)
				{
					finish_count = 0;
					arm.claw_set(90, 0);
					delay(100);
					running_state = 4;
					break;
				}
				finish_count = 0;
			}
    }break;
    case 4:{
      arm.coordinate_set(15.0f, 0, 15, 0, -90, 90, 1000);
			while(1)
			{
				for(uint8_t i = 3; i < 7; i++)
				{
					if(arm.knot_finish(i))
					{
						finish_count++;
					}
				}
				if(finish_count == 4)
				{
					finish_count = 0;
					running_state = 0;
					break;
				}
				finish_count = 0;					
			}
      delay(2000);
    }break;
  }
  delay(100);
}

