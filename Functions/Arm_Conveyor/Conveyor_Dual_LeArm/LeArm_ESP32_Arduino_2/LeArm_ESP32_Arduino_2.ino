#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "WonderMV.hpp"
#include "ASR_module.hpp"

Led_t led_obj;
Buzzer_t buzzer_obj;
LeArm_t arm;
IIC iic;
WonderMV mv;
ASR_MOUDLE asr;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  led_obj.init(IO_LED);
  buzzer_obj.init(IO_BUZZER);
  iic.init();
  mv.init(&iic);
  asr.init(&iic);

  Serial.begin(9600);
  arm.coordinate_set(2, -15 , 2 , -90 , -90 , 90 , 1000);
  delay(2000);
  arm.claw_set(90 , 100);
}

uint8_t step = 0;
MV_RESULT_ST result;

void loop() {
  switch(step){
    case 0:{
      delay(100);
      mv.get_result(OBJECT_REG, &result);
      if(result.id != 0){
        delay(4000);
        arm.coordinate_set(2, -19, -1, 0, -90, 90, 600);
        delay(800);
        arm.claw_set(30 , 100);
        delay(500);
        arm.coordinate_set(2, -15, 10, 0, -90, 90, 600);
        delay(800);
        step = 1;
      }
    }break;
    case 1:{
      if(result.id == 1){
        arm.coordinate_set(2, 15, -5, 0, -90, 90, 1000);
        delay(1500);
        arm.claw_set(90, 100);
        delay(500);
      }else if(result.id == 2){
        arm.coordinate_set(8, 15, -5, 0, -90, 90, 1000);
        delay(1500);
        arm.claw_set(90, 100);
        delay(500);
      }else if(result.id == 3){
        arm.coordinate_set(15, 15, -5, 0, -90, 90, 1000);
        delay(1500);
        arm.claw_set(90, 100);
        delay(500);
      }else if(result.id == 4){
        arm.coordinate_set(15, 0, -5, 0, -90, 90, 1000);
        delay(1500);
        arm.claw_set(90, 100);
        delay(500);
      }
      arm.coordinate_set(2, -15 , 2 , -90 , -90 , 90 , 1000);
      delay(1500);
      mv.get_result(OBJECT_REG, &result); //清空识别搬运过程中识别到的结果
      result.id = 0;
      step = 0;
    }break;
    default:break;
  }
}
