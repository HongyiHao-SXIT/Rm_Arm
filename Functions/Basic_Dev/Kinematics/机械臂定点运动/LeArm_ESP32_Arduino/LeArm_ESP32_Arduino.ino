#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"

LeArm_t arm;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();

  Serial.begin(9600);
  delay(2000);
  Serial.println("start");
}

void loop() {
  arm.coordinate_set(20 , -15 , 20 , 0 , -90, 90 , 1000);
  delay(2000);
  arm.coordinate_set(20, 15, 20, 0, -90 , 90, 1000);
  delay(2000);
  arm.coordinate_set(20, 0, 5, 0, -90, 90, 1000);
  delay(2000);
}

