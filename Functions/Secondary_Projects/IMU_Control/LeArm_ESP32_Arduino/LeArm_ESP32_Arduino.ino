#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "IMU.hpp"

Buzzer_t buzzer_obj;
LeArm_t arm;

IIC iic;
IMU imu;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源
  Serial.begin(9600);
  arm.init();
  buzzer_obj.init(IO_BUZZER);

  iic.init();
  imu.init(&iic);

  arm.coordinate_set(15,0,10,0,-90,90, 1000);
  arm.claw_set(90, 500);
  buzzer_obj.blink(1500 , 50, 50, 1);
  Serial.println("begin");
  delay(1000);
}

float euler[3];
uint8_t setp = 0;

void loop() {
  imu.get_euler(euler);
  // Serial.printf("euler: %f, %f\n", euler[0], euler[1]);
  switch(setp){
    case 0:
      if(euler[0] < -30){
        buzzer_obj.blink(1500 , 50, 50, 1);
        delay(500);
        arm.coordinate_set(20,0,-4,0,-90,90, 1000);
        delay(1200);
        arm.claw_set(30, 500);
        delay(1000);
        arm.coordinate_set(15,0,10,0,-90,90, 1000);
        delay(1200);
        setp = 1;
      }
      break;
    case 1:
      if(euler[1] < -30){
        buzzer_obj.blink(1500 , 50, 50, 1);
        delay(500);
        arm.coordinate_set(1,15,10,0,-90,90, 1000);
        delay(1200);
        arm.coordinate_set(1,20,-4,0,-90,90, 1000);
        delay(1200);
        arm.claw_set(90, 500);
        delay(1000);
        arm.coordinate_set(1,15,10,0,-90,90, 1000);
        delay(1200);
        arm.coordinate_set(15,0,10,0,-90,90, 1000);
        delay(1200);
        setp = 0;
      }else if(euler[1] > 30){
        buzzer_obj.blink(1500 , 50, 50, 1);
        delay(500);
        arm.coordinate_set(1,-15,10,0,-90,90, 1000);
        delay(1200);
        arm.coordinate_set(1,-20,-4,0,-90,90, 1000);
        delay(1200);
        arm.claw_set(90, 500);
        delay(1000);
        arm.coordinate_set(1,-15,10,0,-90,90, 1000);
        delay(1200);
        arm.coordinate_set(15,0,10,0,-90,90, 1000);
        delay(1200);
        setp = 0;
      }
      break;
  }
  delay(50);
}

