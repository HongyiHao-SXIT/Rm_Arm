#include "PC_BLE_CTL.hpp"
#include "string.h"
#include "./../../Config.h"

#define SERIAL_PC_BLE Serial

uint8_t rx_dma_buf[128];
uint8_t rx_fifo[256];

uint8_t PC_BLE_CTL::transmit_data(const uint8_t* pdata, uint16_t size)
{
  return (uint8_t)SERIAL_PC_BLE.write(pdata , size);
}

void PC_BLE_CTL::packet_transmit(uint8_t* data, uint8_t len)
{
  uint8_t send_data[128];
  send_data[0] = APP_PACKET_HEADER;
  send_data[1] = APP_PACKET_HEADER;
  send_data[2] = len+1;
  for(int i = 0 ; i < len ; i++){
    send_data[i+3] = data[i];
  }
  transmit_data(send_data, len+3);
}


void PC_BLE_CTL::unpack(void)
{
  uint8_t data[MAX_PACKET_LENGTH];
  size_t readlen = 0;
  
  static uint8_t buffer_index = 0;
  uint8_t rec = 0;
  while (SERIAL_PC_BLE.available() > 0) 
  {
    rec = SERIAL_PC_BLE.read();
    switch(packet_status)
    {
      case PACKET_HEADER_1:
        packet_status = rec == APP_PACKET_HEADER ? PACKET_HEADER_2: PACKET_HEADER_1;
        tmp_packet.packet_header[0] = rec;
        break;

      case PACKET_HEADER_2:
        packet_status = rec == APP_PACKET_HEADER ? PACKET_DATA_LENGTH: PACKET_HEADER_1;
        tmp_packet.packet_header[1] = rec;
        break;

      case PACKET_DATA_LENGTH:
        packet_status = rec != 0 ? PACKET_FUNCTION: PACKET_HEADER_1;
        tmp_packet.data_len = rec;
        break;

      case PACKET_FUNCTION:
        packet_status = rec < APP_FUNC_NULL ? PACKET_DATA : PACKET_HEADER_1;
        tmp_packet.cmd = rec;
        if(tmp_packet.data_len == 2)
        {
          memcpy(&packet , &tmp_packet , sizeof(PacketObjectTypeDef));
          unpack_successful = true;
          packet_status = PACKET_HEADER_1;
        }
        buffer_index = 0;
        break;

      case PACKET_DATA:
        tmp_packet.buffer[buffer_index] = rec;
        buffer_index++;
        if(buffer_index >= tmp_packet.data_len - 2)
        {
          memcpy(&packet , &tmp_packet , sizeof(PacketObjectTypeDef));
          unpack_successful = true;
          packet_status = PACKET_HEADER_1;
        }
        break;

      default:
        packet_status = PACKET_HEADER_1;
        break;
    }
    if(unpack_successful)
    {
      return;
    }
  }
}

void PC_BLE_CTL::init(int pc_ble_flag)
{
  running_time = 200;
  unpack_successful = false;
  pinMode(IO_BLE_CTL, OUTPUT);
  /* 控制蓝牙是否上电 */
  if(pc_ble_flag == 0) // PC控制
  {
    digitalWrite(IO_BLE_CTL, LOW);
  }else{ // 蓝牙控制
    digitalWrite(IO_BLE_CTL, HIGH);
  }
  pose.x = 15;
  pose.y = 0;
  pose.z = 2;
  pose.pitch = -30;
}

/* header header len cmd count time [id duty] */
void PC_BLE_CTL::PC_BLE_Task(LeArm_t* robot,Led_t* led,Buzzer_t* buzzer,Motor* motors)
{
  static uint8_t car_run_flag = 0;
  static uint32_t last_car_run_time = 0;
  uint8_t send_data[30] = {0};
  static int8_t motors_speed[4] = {0,0,0,0};

  unpack();
  if(unpack_successful)
  {
    unpack_successful = false;
    if(packet.cmd == APP_ACTION_GROUP_RUN)
    {
      robot->action_group_reset();
    }
    status = (AppFunctionStatus)packet.cmd;
  }

  switch (status)
  {
      case APP_VERSION_QUERY:{
        send_data[0] = APP_VERSION_QUERY;
        send_data[1] = SERVO_TYPE;
        send_data[2] = SOFTWARE_VERSION;
        delay(50);
        packet_transmit(send_data, 3);
        memset(send_data, 0, sizeof(send_data));
        status = APP_FUNC_NULL;
      }break;

      case APP_SERVO_OFFSET_READ:{
        send_data[0] = APP_SERVO_OFFSET_READ;
        for (uint8_t i = 0; i < 6; i++)
        {
          send_data[(2 * i) + 1] = 1 + i;
          send_data[(2 * i) + 2] = (uint8_t)robot->offset_read(1 + i);
        }
        packet_transmit(send_data, 13);
        memset(send_data, 0, sizeof(send_data));
        status = APP_FUNC_NULL;
      }break;
  
      case APP_MULT_SERVO_MOVE:{
          servos_count = packet.buffer[0];
          servos_count = packet.buffer[0];
          running_time = MERGE_HL(packet.buffer[2], packet.buffer[1]);
          for (uint8_t i = 0; i < servos_count; i++)
          {
              set_id = packet.buffer[3 + i * 3];
              set_duty = MERGE_HL(packet.buffer[5 + i * 3], packet.buffer[4 + i * 3]);
#if (SERVO_TYPE == TYPE_PWM_SERVO)
              // if(set_id == 6){
              //   robot->knot_run(set_id, set_duty, 300);
              // }else{
                robot->knot_run(set_id, set_duty, running_time);
              // }
#else
              robot->knot_run(set_id, set_duty, running_time);
#endif
          }
          status = APP_FUNC_NULL;
      }break;

      case APP_COORDINATE_SET:{
          static uint32_t last_time = 0;
          float x = (int8_t)packet.buffer[0]/10.0f;
          float y = (int8_t)packet.buffer[1]/10.0f;
          float z = (int8_t)packet.buffer[2]/10.0f;
          if(robot->coordinate_set(pose.x+x , pose.y+y , pose.z+z , pose.pitch , -90 , 0 , 50))
          {
            pose.x += x;
            pose.y += y;
            pose.z += z;
          }else{
            if(last_time < millis()){
              last_time = millis() + 200;
              buzzer->blink(1000, 50, 150, 1);
            }
          }
          status = APP_FUNC_NULL;
      }break;

      case APP_ACTION_GROUP_RUN:{
          packet.action_group_index = packet.buffer[0];
          packet.running_times = MERGE_HL(packet.buffer[2], packet.buffer[1]);
          if(robot->action_group_run(packet.action_group_index, packet.running_times))
          {
            status = APP_FUNC_NULL;
          }
      }break;
          
      case APP_FULL_ACTION_STOP:{
          robot->action_group_stop();
          status = APP_FUNC_NULL;
      }break;
      
      case APP_FULL_ACTION_ERASE:{
          robot->action_group_erase();
          delay(5);
          uint8_t cmd[1] = {APP_FULL_ACTION_ERASE};
          packet_transmit(cmd, 1);
          status = APP_FUNC_NULL;
      }break;

      case APP_CHASSIS_CONTROL:{
          int x = 0, y = 0, rx = 0;
          car_run_flag = 1;
          last_car_run_time = millis() + 150;
          switch(packet.buffer[0]){
            case 0: //停止
              break;
            case 1: //左平移
              x = -100;
              break;
            case 2: //左上平移
              x = -50;
              y = 50;
              break;
            case 3: //前进
              y = 100;
              break;
            case 4: //右上平移
              x = 50;
              y = 50;
              break;
            case 5: //右平移
              x = 100;
              break;
            case 6: //右下平移
              x = 50;
              y = -50;
              break;
            case 7: //后退
              y = -100;
              break;
            case 8: //左下平移
              x = -50;
              y = -50;
              break;
          }
          switch(packet.buffer[1]){
            case 0: //停止
              break;
            case 1: //左旋转
              rx = -100;
              break;
            case 2: //右旋转
              rx = 100;
              break;
          }
          // 计算四个轮子的速度
          int Forward_Left = y + x; // 前左轮：前后 + 左右
          int Forward_Right = y - x; // 前右轮：前后 - 左右
          int Backward_Left = y - x; // 后左轮：前后 - 左右
          int Backward_Right = y + x; // 后右轮：前后 + 左右
          if(x == 0 && y == 0)
          {
            Forward_Left = rx;
            Forward_Right = -rx;
            Backward_Left = rx;
            Backward_Right = -rx;
          }else if(rx < 0){
            Forward_Left = (Forward_Left*(100+rx)/100);
            Backward_Left = (Backward_Left*(100+rx)/100);
          }else{
            Forward_Right = (Forward_Right*(100-rx)/100);
            Backward_Right = (Backward_Right*(100-rx)/100);
          }
          motors_speed[0] = Forward_Right;
          motors_speed[1] = -Backward_Right;
          motors_speed[2] = -Forward_Left;
          motors_speed[3] = Backward_Left;
          status = APP_FUNC_NULL;
      }break;

      case APP_SERVO_OFFSET_SET:{
			  robot->offset_set(packet.buffer[0], (int8_t)packet.buffer[1]);
			  status = APP_FUNC_NULL;
      }break;

      case APP_SERVO_OFFSET_DOWNLOAD:{
        robot->offset_save();
        send_data[0] = APP_SERVO_OFFSET_DOWNLOAD;
        delay(50);
        packet_transmit(send_data , 1);
        status = APP_FUNC_NULL;
      }break;
      
      case APP_SERVOS_RESET:{
          robot->reset(1000);
          pose.x = DEFAULT_X;
          pose.y = DEFAULT_Y;
          pose.z = DEFAULT_Z;
          pose.pitch = -30;
          delay(5);
          status = APP_FUNC_NULL;
      }break;
      
      case APP_SERVOS_READ:{
        send_data[0] = APP_SERVOS_READ;
        for (uint8_t i = 0; i < 6; i++)
        {
          send_data[i*3+1] = 1 + i;
          uint16_t duty = robot->knot_read(1 + i);
          send_data[i*3+2] = duty&0xFF;
          send_data[i*3+3] = (duty>>8)&0xFF;
        }
        packet_transmit(send_data, 19);
        memset(send_data, 0, sizeof(send_data));
        delay(5);
        status = APP_FUNC_NULL;
      }break;
      
      case APP_ACTION_DOWNLOAD:{
          packet.action_group_index = packet.buffer[0];
          packet.action_frame_sum = packet.buffer[1];
          packet.action_frame_index = packet.buffer[2];
          if(robot->action_group_save(packet.action_group_index, packet.action_frame_sum, 
                                      packet.action_frame_index, packet.buffer + 3,ACTION_FRAME_SIZE)>0)
          {
            // 若动作组下载成功
            buzzer->blink(1500 , 100 , 50 , 1);
          }
          delay(5);
          uint8_t cmd[1] = {APP_ACTION_DOWNLOAD};
          packet_transmit(cmd, 1);
          status = APP_FUNC_NULL;
      }break;

      default:
          status = APP_FUNC_NULL;
          break;
  }

  static uint32_t last_time_motor = 0;
  if(last_car_run_time < millis() && car_run_flag == 1){
    car_run_flag = 0;
    for(int i = 0 ; i < 4 ; i++){
      motors_speed[i] = 0;
    }
  }
  if(last_time_motor < millis()){
    last_time_motor = millis() + 100;
    motors->set_speed(motors_speed);
  }
  
  delay(10);
}
