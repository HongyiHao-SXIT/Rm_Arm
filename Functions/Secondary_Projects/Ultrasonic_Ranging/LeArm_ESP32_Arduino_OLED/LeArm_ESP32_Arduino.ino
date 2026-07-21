#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include <U8g2lib.h>
#include "Ultrasound.hpp"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE,  /* clock=*/IIC_SCL, /* data=*/IIC_SDA); //实例化对象

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
  u8g2.begin(); //初始化库
  /*****************************************/
  //设置显示旋转为180°
  //（U8G2_R0：不做旋转，U8G2_R1：旋转90°，U8G2_R2：旋转180°，U8G2_R3：旋转270°）
  /*******************************************/
  u8g2.setDisplayRotation(U8G2_R2);

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
        delay(2000);
        dis = ult.GetDistance();
        u8g2.firstPage(); //创建一个画布，用来发送给OLED传感器进行显示。
        do {
          u8g2.setFont(u8g2_font_fur20_tf); 
          u8g2.drawStr(0,20,"distance:");    //设置显示的内容
          u8g2.setCursor(5,50); //切换光标位置，后面新增的字符将会从新的光标的位置开始显示
          u8g2.println(dis);   //从新的光标位置，设置显示内容
          u8g2.drawStr(60,50,"mm");    //设置显示的内容
        }while(u8g2.nextPage());
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
          delay(1000);
        }
      }
    }break;
    case 1:{
      arm.claw_set(30, 200);
			delay(1000);
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
  u8g2.firstPage(); //创建一个画布，用来发送给OLED传感器进行显示。
  do {
    u8g2.setFont(u8g2_font_fur20_tf); 
    u8g2.drawStr(0,20,"distance:");    //设置显示的内容
    u8g2.setCursor(5,50); //切换光标位置，后面新增的字符将会从新的光标的位置开始显示
    u8g2.println(dis);   //从新的光标位置，设置显示内容
    u8g2.drawStr(60,50,"mm");    //设置显示的内容
  }while(u8g2.nextPage());
  delay(100);
}

