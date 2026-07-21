#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "./src/PS2/PS2_CTL.hpp"
#include "stepper.hpp"

Led_t led_obj;
Buzzer_t buzzer_obj;
LeArm_t arm;
PS2_CTL ps2;
IIC iic;
Stepper stepper_obj;

int stepper_run = 0;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  led_obj.init(IO_LED);
  buzzer_obj.init(IO_BUZZER);
  ps2.init();
  iic.init();
  stepper_obj.init(&iic);
  stepper_obj.driver_mode(0x02);
  stepper_obj.reset();
  Serial.begin(9600);

  delay(2000);
}

void loop() {
  ps2.PS2_Task(&arm, &led_obj, &buzzer_obj, &stepper_run);
  stepper_obj.set_step(stepper_run);
}
