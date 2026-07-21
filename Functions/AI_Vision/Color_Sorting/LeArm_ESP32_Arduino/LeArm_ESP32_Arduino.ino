#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "WonderMV.hpp"
#include "Ultrasound.hpp"

Led_t led_obj;
Buzzer_t buzzer_obj;
Button_t key_obj;
LeArm_t arm;

IIC iic;
WonderMV mv;
Ultrasound ult;


#define ULTRASOUND_OFFSET 9.0f

MV_RESULT_ST result;
uint8_t running_state = 0;
uint16_t dis = 0;
uint16_t finish_count = 0;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为高电平时，断开蓝牙模块电源
  
  arm.init();
  led_obj.init(IO_LED);
  buzzer_obj.init(IO_BUZZER);

  iic.init();
  mv.init(&iic);
  ult.init(&iic);

  Serial.begin(9600);
  buzzer_obj.blink(1500, 100, 100, 1);

  ult.Color(0,0,0,0,0,0);
  arm.action_run(0,1);
  delay(3000);
}



void loop() {
  switch(running_state)
	{
    case 0:{
      mv.get_result(COLOR_REG , &result);
      if(result.id != 0)
      {
        delay(500);
        mv.get_result(COLOR_REG , &result);
        if(result.id != 0)
        {
          running_state = 1;
          delay(1000);
          buzzer_obj.blink(1500, 100, 100, 1);
          delay(500);
          break;
        }
      }
      delay(10);
    }break;

    /* 夹取物体 */
    case 1:{
      dis = ult.GetDistance();
      if(dis >= 50 && dis <= 100)
      {
        delay(500);
        dis = ult.GetDistance();
        if(dis >= 50 && dis <= 100)
        {
          arm.coordinate_set((float)(dis / 10) + ULTRASOUND_OFFSET, 0, -7, 0, -90, 90, 1000);
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
              running_state = 2;
              break;
            }
            finish_count = 0;
          }
        }
      }
    }break;
    
    /* 夹取 */
    case 2:{
      arm.claw_set(30, 200);
      delay(200);
      running_state = 3;
    }break;
    
    /* 回到原位 */
    case 3:{
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
          running_state = 4;
          break;
        }
        finish_count = 0;
      }		
    }break;
    
    /* 根据颜色来放置物体 */
    case 4:{
      switch(result.id)
      {
        /* 红 */
        case 1:
          arm.coordinate_set(5.0f, 12.0f, -5, 0, -90, 90, 1000);
          break;
        
        /* 绿 */
        case 2:
          arm.coordinate_set(5.0f, -12.0f, -5, 0, -90, 90, 1000);
          break;
        
        /* 蓝 */
        case 3:
          arm.coordinate_set(30.0f, 0, -5, -20, -90, 0, 1000);
          break;
      }
      
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
          delay(500);
          running_state = 5;
          break;
        }
        finish_count = 0;					
      }
    }break;		
    
    /* 复位 */
    case 5:{
      arm.coordinate_set(12.0f, 0, -2.0f, 0, -90, 90, 1000);
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
    }break;
	}
}

