#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "PC_BLE_CTL.hpp"

Led_t led_obj;
Buzzer_t buzzer_obj;
LeArm_t arm;
PC_BLE_CTL pc_ble_obj;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  led_obj.init(IO_LED);
  buzzer_obj.init(IO_BUZZER);

  Serial.begin(9600);
  delay(2000);

  pc_ble_obj.init(0);

  buzzer_obj.blink(1500, 100, 100, 1);
}

void loop() {
  pc_ble_obj.PC_BLE_Task(&arm , &led_obj , &buzzer_obj);
}
