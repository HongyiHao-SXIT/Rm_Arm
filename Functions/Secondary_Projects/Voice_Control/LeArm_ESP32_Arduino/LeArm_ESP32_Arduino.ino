#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"

Buzzer_t buzzer_obj;
LeArm_t arm;
Led_t led_obj;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为高电平时，断开蓝牙模块电源

  arm.init();
  buzzer_obj.init(IO_BUZZER);
  led_obj.init(IO_LED);

  Serial.begin(9600);
  delay(2000);

  buzzer_obj.blink(1500, 100, 100, 1);
}

void loop() {
  int adcValue = analogRead(PA5);
  if(adcValue > 50){
    led_obj.blink(200, 200, 2);
    buzzer_obj.blink(1500, 100, 100, 1);
    arm.action_run(12, 1);
    delay(2000);
  }
  delay(10);
}

