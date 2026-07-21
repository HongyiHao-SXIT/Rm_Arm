#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "stepper.hpp"
#include "WonderMV.hpp"

Led_t led_obj;
Buzzer_t buzzer_obj;
LeArm_t arm;
IIC iic;
Stepper stepper_obj;
WonderMV mv;

MV_RESULT_ST result;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  led_obj.init(IO_LED);
  buzzer_obj.init(IO_BUZZER);
  iic.init();
  stepper_obj.init(&iic);
  mv.init(&iic);
  stepper_obj.driver_mode(0x02);
  stepper_obj.reset();
  Serial.begin(9600);
  delay(2000);
  stepper_obj.set_step(1000);
  delay(1500);
  arm.coordinate_set(14, 0, -9, 0, -90, 90, 1000); //识别位置
  Serial.println("start");
}


uint16_t step_num = 0;

void loop() {
  mv.get_result(COLOR_REG , &result);
  if(result.id != 0)
  {
    delay(3000);
    mv.get_result(COLOR_REG , &result);
    if(result.id != 0)
    {
      buzzer_obj.blink(1500, 100, 100, 1);
      delay(500);
      arm.coordinate_set(15, 0, -13, 0, -90, 90, 1000); //准备夹取
      delay(1500);
      arm.claw_set(30, 200); //夹取
      delay(400);
      arm.coordinate_set(15.0f, 0, 15, 0, -90, 90, 1000); //复位
      delay(1500);
      step_num = 1000+(result.id*1000);
      step_num = step_num > 5000 ? 5000 : step_num;
      stepper_obj.set_step(step_num); //运动到对应位置
      delay(step_num);
      arm.coordinate_set(15, 0, -11, 0, -90, 90, 1000); //准备放下
      delay(1500);
      arm.claw_set(90, 200); //放下
      delay(400);
      arm.coordinate_set(15.0f, 0, 15, 0, -90, 90, 1000); //复位
      delay(1500);
      stepper_obj.set_step(-step_num);
      delay(step_num);
      arm.coordinate_set(14, 0, -9, 0, -90, 90, 1000); //识别位置
      delay(1500);
      step_num = 0;
    }
  }
  delay(100);
}
