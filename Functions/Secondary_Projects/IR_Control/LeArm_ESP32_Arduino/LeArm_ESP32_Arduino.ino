#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"

LeArm_t arm;
Led_t led_obj;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  led_obj.init(IO_LED);
  Serial.begin(9600);
  delay(2000);

  pinMode(PA5, INPUT);
  pinMode(IO13, INPUT);
}

void loop() {
  if(digitalRead(PA5) == LOW) //夹取左边到中间
  {
    Serial.println("PA5");
    led_obj.blink(200 , 200 , 1);
    arm.action_run(8 , 1);
  }else if(digitalRead(IO13) == LOW) //夹取右边到中间
  {
    led_obj.blink(100, 100, 2);
    Serial.println("IO13");
    arm.action_run(11, 1);
  }
  delay(100);
}
