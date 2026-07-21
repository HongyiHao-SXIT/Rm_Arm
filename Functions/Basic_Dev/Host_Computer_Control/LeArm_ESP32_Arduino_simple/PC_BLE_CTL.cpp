#include "PC_BLE_CTL.hpp"
#include "string.h"
#include "Config.h"

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
void PC_BLE_CTL::PC_BLE_Task(LeArm_t* robot,Led_t* led,Buzzer_t* buzzer)
{
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

  uint8_t send_data[30] = {0};
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

      case APP_MULT_SERVO_MOVE:{
          servos_count = packet.buffer[0];
          servos_count = packet.buffer[0];
          running_time = MERGE_HL(packet.buffer[2], packet.buffer[1]);
          for (uint8_t i = 0; i < servos_count; i++)
          {
              set_id = packet.buffer[3 + i * 3];
              set_duty = MERGE_HL(packet.buffer[5 + i * 3], packet.buffer[4 + i * 3]);
              robot->knot_run(set_id, set_duty, running_time);
          }
          status = APP_FUNC_NULL;
      }break;

      default:
          status = APP_FUNC_NULL;
          break;
  }

  delay(10);
}
