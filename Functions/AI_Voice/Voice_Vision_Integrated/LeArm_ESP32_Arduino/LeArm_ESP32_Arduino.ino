#include "Config.h"
#include "Hiwonder.hpp"
#include "Robot_arm.hpp"
#include "IIC.hpp"
#include "WonderMV.hpp"
#include "ASR_module.hpp"


Led_t led_obj;
Buzzer_t buzzer_obj;
Button_t key_obj;
LeArm_t arm;

IIC iic;
WonderMV mv;
ASR_MOUDLE asr;

float coord[7][3] = {
 {14,0,-3}, //复位坐标
 {17,0,-6}, //夹取
 {15,0,15}, //夹取过渡位置
 {3,-18,10}, //放一
 {15,-18,10}, //放二
 {3,18,10}, //放三
 {15,18,10}, //放四
};

MV_RESULT_ST result;
void setup() {
  delay(1000);
  pinMode(IO_BLE_CTL, OUTPUT);
  digitalWrite(IO_BLE_CTL, LOW);  // 设置蓝牙控制引脚为高电平时，断开蓝牙模块电源
  
  arm.init();
  led_obj.init(IO_LED);
  buzzer_obj.init(IO_BUZZER);

  iic.init();
  mv.init(&iic);
  asr.init(&iic);

  Serial.begin(9600);
  buzzer_obj.blink(1500, 100, 100, 1);

  arm.coordinate_set(coord[0][0],coord[0][1],coord[0][2],0,-90,90,1000); //复位
  delay(3000);
}

uint8_t asr_result = 0;
uint8_t open_flag = 0;
void loop() {
  asr_result = asr.rec_recognition();
  if(asr_result == 0x64)
  {
    open_flag = 1;
    buzzer_obj.blink(1500, 100, 50, 1);
    led_obj.on_off(1);
  }else if(asr_result == 0x7B){
    open_flag = 0;
    buzzer_obj.blink(1500, 300, 50, 1);
    led_obj.on_off(0);
  }
  asr_result = 0;

  if(open_flag == 1){
    mv.get_result(OBJECT_REG , &result);
    if(result.id != 0){
      Serial.println(result.id);
      delay(2000);
      mv.get_result(OBJECT_REG , &result);
      if(result.id == 1){ /* 识别为可回收垃圾 */
        Serial.println("1");
        asr.speak(ASR_ANNOUNCER , 1);
        arm.coordinate_set(coord[1][0],coord[1][1],coord[1][2],0,-90,90,1200); //夹取
        delay(1500);
        arm.claw_set(30,200); //夹取
        delay(300);
        arm.coordinate_set(coord[2][0],coord[2][1],coord[2][2],0,-90,90,1000); //夹取过渡位置
        delay(1300);
        arm.coordinate_set(coord[3][0],coord[3][1],coord[3][2],0,-90,90,1000); //放一
      }else if(result.id == 2){ /* 识别为有害垃圾 */
        Serial.println("2");
        asr.speak(ASR_ANNOUNCER , 3);
        arm.coordinate_set(coord[1][0],coord[1][1],coord[1][2],0,-90,90,1200); //夹取
        delay(1500);
        arm.claw_set(30,200); //夹取
        delay(300);
        arm.coordinate_set(coord[2][0],coord[2][1],coord[2][2],0,-90,90,1000); //夹取过渡位置
        delay(1300);
        arm.coordinate_set(coord[4][0],coord[4][1],coord[4][2],0,-90,90,1000); //放二
      }else if(result.id == 3){ /* 识别为厨余垃圾 */
        Serial.println("3");
        asr.speak(ASR_ANNOUNCER , 2);
        arm.coordinate_set(coord[1][0],coord[1][1],coord[1][2],0,-90,90,1200); //夹取
        delay(1500);
        arm.claw_set(30,200); //夹取
        delay(300);
        arm.coordinate_set(coord[2][0],coord[2][1],coord[2][2],0,-90,90,1000); //夹取过渡位置
        delay(1300);
        arm.coordinate_set(coord[5][0],coord[5][1],coord[5][2],0,-90,90,1000); //放一
      }else if(result.id == 4){ /* 识别为其他垃圾 */
        Serial.println("4");
        asr.speak(ASR_ANNOUNCER , 4);
        arm.coordinate_set(coord[1][0],coord[1][1],coord[1][2],0,-90,90,1200); //夹取
        delay(1500);
        arm.claw_set(30,200); //夹取
        delay(300);
        arm.coordinate_set(coord[2][0],coord[2][1],coord[2][2],0,-90,90,1000); //夹取过渡位置
        delay(1300);
        arm.coordinate_set(coord[6][0],coord[6][1],coord[6][2],0,-90,90,1000); //放一
      }
      delay(1300);
      arm.claw_set(90,100); //放
      delay(500);
      arm.coordinate_set(coord[0][0],coord[0][1],coord[0][2],0,-90,90,1000); //复位
      delay(1300);
      result.id = 0;
    }
  }
  delay(100);
}

