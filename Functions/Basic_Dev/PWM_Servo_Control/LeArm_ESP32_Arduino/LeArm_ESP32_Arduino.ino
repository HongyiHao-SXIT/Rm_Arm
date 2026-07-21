#include "Config.h"
#include "Robot_arm.hpp"

LeArm_t arm;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源
  arm.init();
  Serial.begin(9600);
  delay(2000);
}

void loop() {
  arm.knot_run(1 , 500 , 1000);
  delay(1000);
  arm.knot_run(1, 1500 , 2000);
  delay(2000);
}

