#include "Config.h"
#include "Hiwonder.hpp"

BusServo_t busservo_obj;

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  Serial1.begin(115200 ,SERIAL_8N1 , BUS_RX , BUS_TX);
  busservo_obj.init(&Serial1);

  Serial.begin(9600);
  delay(2000);
}

void loop() {
  busservo_obj.set_angle(1, 0, 1000);
  delay(1100);
  Serial.println(busservo_obj.read_angle(1));
  delay(500);
  busservo_obj.set_angle(1, 180, 2000);
  delay(2100);
  Serial.println(busservo_obj.read_angle(1));
  delay(500);
}

