#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE,  /* clock=*/IIC_SCL, /* data=*/IIC_SDA); //实例化对象

LeArm_t arm;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为高电平时，断开蓝牙模块电源

  arm.init();
  Serial.begin(9600);
  arm.action_run(0,1);
  delay(3000);

  u8g2.begin(); //初始化库
  /*****************************************/
  //设置显示旋转为180°
  //（U8G2_R0：不做旋转，U8G2_R1：旋转90°，U8G2_R2：旋转180°，U8G2_R3：旋转270°）
  /*******************************************/
  u8g2.setDisplayRotation(U8G2_R2); 
}

void loop() {
  u8g2.firstPage(); //创建一个画布，用来发送给OLED传感器进行显示。
  do {
    u8g2.setFont(u8g2_font_fur20_tf); //设置字体
    u8g2.drawStr(0,20,"Hiwonder");    //设置显示的内容
    // u8g2.setFont(u8g2_font_courB18_tf); 
    // u8g2.setCursor(0,50); //切换光标位置，后面新增的字符将会从新的光标的位置开始显示
    // u8g2.println(1234);   //从新的光标位置，设置显示内容
  }
  while ( u8g2.nextPage() );
  delay(100);
}

