#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"

LeArm_t arm;


/* 
  * 一个的动作帧的数组内容
  * 控制的舵机数量：frame[0]
  * 运行时间：frame[1] + frame[2] << 8
  * 舵机id：frame[3 + i * 3]:
  * 舵机脉宽：frame[4 + i * 3] + frame[5 + i * 3] << 8
  * 0-动作帧运行失败 1-动作帧运行完成     0100 1101 0001
  */
#pragma pack(1)
  typedef struct {
    uint8_t id;       // 舵机ID，对应frame[3 + i*3]
    uint16_t duty;    // 舵机脉宽，对应frame[4 + i*3] + (frame[5 + i*3] << 8)
  } ServoConfig;

  typedef struct {
    uint8_t num;        // 舵机数量，对应frame[0]
    uint16_t time;      // 运行时间，对应frame[1] + (frame[2] << 8)
    ServoConfig configs[6]; // 舵机配置数组（柔性数组）
  } ActionFrame;
#pragma pack(0)

void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为低电平时，断开蓝牙模块电源

  arm.init();
  Serial.begin(9600);
  delay(2000);

  // 示例：初始化6个舵机
  ActionFrame act = {
    .num = 6,
    .time = 1500,
    .configs = {
        {1, 1500}, {2, 1500}, {3, 1500},
        {4, 1500}, {5, 1500}, {6, 1500}
    }
  };
  arm.action_group_save(30 , 1 , 0 , (uint8_t*)&act , ACTION_FRAME_SIZE);
  delay(1000);
  arm.action_run(30, 1);
  delay(1000);

  act.configs[0].duty = 800;
  act.configs[1].duty = 500;
  arm.action_group_save(31 , 1 , 0 , (uint8_t*)&act , ACTION_FRAME_SIZE);
  delay(1000);
  arm.action_run(31, 1);
  delay(1000);
}

void loop() {
  arm.action_run(30, 1);
  delay(1000);
  arm.action_run(31, 1);
  delay(1000);
}
