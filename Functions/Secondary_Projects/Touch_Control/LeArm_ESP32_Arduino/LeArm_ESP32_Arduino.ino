#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE,  /* clock=*/IIC_SCL, /* data=*/IIC_SDA); //实例化对象

Buzzer_t buzzer_obj;
LeArm_t arm;
Led_t led_obj;

#define NUM_MAX 100
#define NUM_MIN 1

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为高电平时，断开蓝牙模块电源

  arm.init();
  buzzer_obj.init(IO_BUZZER);
  led_obj.init(IO_LED);

  Serial.begin(9600);
  delay(2000);

  u8g2.begin(); //初始化库
  /*****************************************/
  //设置显示旋转为180°
  //（U8G2_R0：不做旋转，U8G2_R1：旋转90°，U8G2_R2：旋转180°，U8G2_R3：旋转270°）
  /*******************************************/
  u8g2.setDisplayRotation(U8G2_R2);

  pinMode(PA5, INPUT);

  buzzer_obj.blink(1500, 100, 100, 1);
}

int num = 15;

void loop() {
  if(digitalRead(PA5) == HIGH){
    led_obj.blink(200, 200, 1);
    buzzer_obj.blink(1500 , 100, 100, 1);
    if(num % 3 == 0)
    {
      arm.action_run(12 , 1);
    }
    else if(num % 7 == 0)
    {
      arm.action_run(13 , 1);
    }
    else
    {
      arm.action_run(14 , 1);
    }
    while(1)
    {
      if(digitalRead(PA5) == LOW)
      {
        break;
      }
    }
  }else{
      num = rand() % (NUM_MAX + 1- NUM_MIN) + NUM_MIN;
  }
  u8g2.firstPage(); //创建一个画布，用来发送给OLED传感器进行显示。
  do {
    u8g2.setFont(u8g2_font_fur20_tf); 
    u8g2.drawStr(10,20,"num:");    //设置显示的内容
    u8g2.setCursor(10,50); //切换光标位置，后面新增的字符将会从新的光标的位置开始显示
    u8g2.println(num);   //从新的光标位置，设置显示内容
  }while(u8g2.nextPage());
  delay(80);
}

