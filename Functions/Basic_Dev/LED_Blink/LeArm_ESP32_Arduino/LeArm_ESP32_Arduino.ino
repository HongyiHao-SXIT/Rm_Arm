#include "Config.h"
#include "Hiwonder.hpp"

Led_t led_obj;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为高电平时，断开蓝牙模块电源

  led_obj.init(IO_LED);
  Serial.begin(9600);

  /* 方法一：调用led对象的blink方法，
  设置LED闪烁，亮灯时长为1s，灭灯的时长为1s，闪烁次数为0 次即一直闪烁 */
  led_obj.blink(1000,1000,0);
}

void loop() {
  /* 方法二：调用led对象的on_off方法，设置LED亮灯，延时1s，灭灯后延时1s */
  // led_obj.on_off(0);
  // delay(1000);
  // led_obj.on_off(1);
  // delay(1000);
  
  delay(100);
}

