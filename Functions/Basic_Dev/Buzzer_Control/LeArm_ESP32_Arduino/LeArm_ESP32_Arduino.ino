#include "Config.h"
#include "Hiwonder.hpp"

Button_t key_obj;
Buzzer_t buzzer_obj;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  buzzer_obj.init(IO_BUZZER);
  key_obj.init();
  Serial.begin(9600);
}

void loop() {
  if(key_obj.read(1) == 1){
    buzzer_obj.blink(1500 , 100 , 100 , 1); // 按键1按下，蜂鸣器响1次
    while(key_obj.read(1) == 1); // 等待按键松开
  }
  delay(50);
}
