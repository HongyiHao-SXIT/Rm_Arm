#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "ASR_module.hpp"
#include "Ultrasound.hpp"

LeArm_t arm;
IIC iic;
ASR_MOUDLE asr;
Ultrasound ult;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  iic.init();
  asr.init(&iic);
  ult.init(&iic);

  Serial.begin(9600);
  delay(2000);
}

void loop() {
  uint16_t distance = ult.GetDistance();
  Serial.println(distance);
  if(distance < 200 && distance > 20)
  {
    ult.Color(250, 0, 0, 250, 0, 0);
    delay(100);
    asr.speak(ASR_ANNOUNCER , 5);
    delay(2000);
  }else{
    ult.Color(0, 250, 0, 0, 250, 0);
    delay(100);
  }
  delay(100);
}

