#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "./src/PC_BLE/PC_BLE_CTL.hpp"
#include "motor.hpp"

Led_t led_obj;
Buzzer_t buzzer_obj;
LeArm_t arm;
PC_BLE_CTL pc_ble_obj;
IIC iic;
Motor motors_obj;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  led_obj.init(IO_LED);
  buzzer_obj.init(IO_BUZZER);
  iic.init();
  motors_obj.init(&iic);
  pc_ble_obj.init(1); // 0：选择PC控制模式 , 1：选择蓝牙控制模式
  Serial.begin(9600);
  delay(2000);

  Serial.println("begin");
  delay(1000);
}

void loop() {
  pc_ble_obj.PC_BLE_Task(&arm , &led_obj , &buzzer_obj , &motors_obj);
}

