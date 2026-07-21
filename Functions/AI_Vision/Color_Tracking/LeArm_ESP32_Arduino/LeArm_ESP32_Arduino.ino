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

#define CENTER_X	160
#define CENTER_Y	120
#define DEADBAND_X	10
#define DEADBAND_Y	10

int x_bias = 0;
int y_bias = 0;
int run_time = 600;

float x_inc_total = 0;
float y_inc_total = 0;

void loop() {
  mv.get_result(COLOR_REG , &result);
  if(result.id == 1){
    x_bias = result.x - CENTER_X;
    y_bias = result.y - CENTER_Y;
    if(abs(x_bias) >= DEADBAND_X)
    {	  
      x_inc_total = map((float)x_bias, -160.0f, 160.0f, -800, 800);
      arm.knot_run(6, (int)(SERVO6_RESET_DUTY - x_inc_total), run_time);
    }
    if(abs(y_bias) >= DEADBAND_Y)
    {	  
      y_inc_total = map((float)y_bias, -120.0f, 120.0f, -300, 300);
      arm.knot_run(3, (int)(SERVO3_RESET_DUTY - (int)y_inc_total), run_time);
    }
  }else
  {
    arm.knot_run(6, SERVO6_RESET_DUTY, 2000);
    arm.knot_run(3, SERVO3_RESET_DUTY, 2000);
  }
  result.id = 0;
  delay(100);
}

