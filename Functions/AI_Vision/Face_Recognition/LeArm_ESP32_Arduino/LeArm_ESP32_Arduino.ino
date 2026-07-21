#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "WonderMV.hpp"

Led_t led_obj;
Buzzer_t buzzer_obj;
Button_t key_obj;
LeArm_t arm;

IIC iic;
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
  mv.init(&iic);

  Serial.begin(9600);
  buzzer_obj.blink(1500, 100, 100, 1);

  arm.action_run(0,1);
  delay(3000);
}


void loop() {
  mv.get_result(FACE_REG , &result);
  if(result.id == 1){
    Serial.println(result.id);
    led_obj.blink(100, 100 , 1);
    buzzer_obj.blink(1500 , 100, 100 , 1);
    arm.action_run(17,1);
    delay(1000);
    result.id = 0;
  }
  delay(100);
}
