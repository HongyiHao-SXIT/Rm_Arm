#include "Config.h"
#include "Hiwonder.hpp"

Button_t key_obj;
Led_t led_obj;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  led_obj.init(IO_LED);
  key_obj.init();
  Serial.begin(9600);
}

void loop() {
  if(key_obj.read(1) == 1){
    led_obj.blink(100 , 100 , 1); // 按键1按下，LED闪烁1次
    while(key_obj.read(1) == 1); // 等待按键松开
  }else if (key_obj.read(2) == 1){
    led_obj.blink(100, 100, 2); // 按键2按下，LED闪烁2次
    while(key_obj.read(2) == 1); // 等待按键松开
  }
  delay(50);
}
