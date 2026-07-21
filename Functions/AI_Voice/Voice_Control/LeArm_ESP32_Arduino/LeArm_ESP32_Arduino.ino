#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "ASR_module.hpp"

LeArm_t arm;
IIC iic;
ASR_MOUDLE asr;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  iic.init();
  asr.init(&iic);

  Serial.begin(9600);
  arm.coordinate_set(20 , 0 , 20, 0 , -90, 90, 1000);
  delay(1500);
  arm.claw_set(90, 100);
  delay(2000);
}

void loop() {
  uint8_t asr_result = asr.rec_recognition();
  if(asr_result == 0x69){
    // 分拣红色
    arm.coordinate_set(5, 12 , -6 , 0 , -90 , 90 , 1000);
    delay(1500);
    arm.claw_set(30, 100);
    delay(300);
    arm.coordinate_set(20 , 0 , 20, 0 , -90, 90, 1000);
    delay(1100);
    arm.coordinate_set(26 , 0 , -6, -90 , -90, 90, 1000);
    delay(1100);
    arm.claw_set(90, 100);
    delay(300);
    arm.coordinate_set(20 , 0 , 20, 0 , -90, 90, 1000);
    delay(1500);
  }else if(asr_result == 0x6A){
    // 分拣绿色
    arm.coordinate_set(5, -12 , -6 , 0 , -90 , 90 , 1000);
    delay(1500);
    arm.claw_set(30, 100);
    delay(300);
    arm.coordinate_set(20 , 0 , 20, 0 , -90, 90, 1000);
    delay(1100);
    arm.coordinate_set(26 , 0 , -6, -90 , -90, 90, 1000);
    delay(1100);
    arm.claw_set(90, 100);
    delay(300);
    arm.coordinate_set(20 , 0 , 20, 0 , -90, 90, 1000);
    delay(1500);
  }else if(asr_result == 0x6B){
    // 分拣蓝色
    arm.coordinate_set(15, 0 , -6 , 0 , -90 , 90 , 1000);
    delay(1500);
    arm.claw_set(30, 100);
    delay(300);
    arm.coordinate_set(20 , 0 , 20, 0 , -90, 90, 1000);
    delay(1100);
    arm.coordinate_set(26 , 0 , -6, -90 , -90, 90, 1000);
    delay(1100);
    arm.claw_set(90, 100);
    delay(300);
    arm.coordinate_set(20 , 0 , 20, 0 , -90, 90, 1000);
    delay(1500);
  }
  delay(200);
}
