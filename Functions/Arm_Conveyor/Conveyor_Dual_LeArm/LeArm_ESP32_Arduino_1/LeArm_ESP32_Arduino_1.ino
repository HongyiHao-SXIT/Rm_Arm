#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "Ultrasound.hpp"
#include "conveyor_belt.hpp"

Led_t led_obj;
Buzzer_t buzzer;
LeArm_t arm;
IIC iic;
CONVEYOR_BELT belt;
Ultrasound ult;

#define ULTRASOUND_OFFSET 10.0f

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  led_obj.init(IO_LED);
  buzzer.init(IO_BUZZER);
  iic.init();
  belt.init(&iic);
  ult.init(&iic);

  Serial.begin(9600);
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
          delay(1200);
          running_state = 1;
        }
      }
      delay(100);
    }break;
    case 1:{ // 抓取
      arm.claw_set(30, 500);
			delay(700);
			running_state = 2;
    }break;
    case 2:{ // 过渡位置
      arm.coordinate_set(15.0f, 0, 15, 0, -90, 90, 900);
			delay(900);
      running_state = 3;
    }break;
    case 3:{ // 放置
      arm.coordinate_set(2.0f, -15, 10, 0, -90, 90, 1000);
      delay(1000);
      arm.coordinate_set(2.0f, -15, 1, 0, -90, 90, 800);
      delay(1000);
      arm.claw_set(90, 0);
      delay(500);
      running_state = 4;
    }break;
    case 4:{
      arm.coordinate_set(2.0f, -15, 10, 0, -90, 90, 800);
      delay(1000);
      belt.set_speed(-100);
      arm.coordinate_set(15.0f, 0, 15, 0, -90, 90, 1000);
      delay(7000); // 等待传送带运行
      belt.set_speed(0);
      running_state = 0;
    }break;
  }
}
