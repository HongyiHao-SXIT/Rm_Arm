#include "PC_BLE_CTL.hpp"
#include "string.h"
#include "./../../Config.h"

#define SERIAL_PC_BLE Serial

// AppHandleTypeDef app;

uint8_t rx_dma_buf[128];
uint8_t rx_fifo[256];

static void bluetooth_switch_write_pin(uint8_t new_state)
{
  // HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, (GPIO_PinState)new_state);
}

uint8_t PC_BLE_CTL::transmit_data(const uint8_t* pdata, uint16_t size)
{
  return (uint8_t)SERIAL_PC_BLE.write(pdata , size);
}

void PC_BLE_CTL::packet_transmit(AppFunctionStatus cmd)
{
  uint8_t data[5];
  data[0] = APP_PACKET_HEADER;
  data[1] = APP_PACKET_HEADER;
  data[2] = 2;
  data[3] = cmd;
  transmit_data(data, 4);
}


void PC_BLE_CTL::unpack(void)
{
  uint8_t data[MAX_PACKET_LENGTH];
  size_t readlen = 0;
  
  static uint8_t buffer_index = 0;
  // if(SERIAL_PC_BLE.available()>0)
  // {
  //   Serial.print("len:");
  //   Serial.println(SERIAL_PC_BLE.available());
  //   // while(SERIAL_PC_BLE.available()>0)
  //   //   Serial.print(SERIAL_PC_BLE.read());
  //   // Serial.println("");
  // }

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
  }
  // if(rec != 0x00)
  // {
  //   Serial.println("");
  // }
}


void PC_BLE_CTL::init(int pc_ble_flag)
{
  running_time = 1000;
  unpack_successful = false;
  pinMode(IO_BLE_CTL, OUTPUT);
  /* 控制蓝牙是否上电 */
  if(pc_ble_flag == 0) // PC控制
  {
    digitalWrite(IO_BLE_CTL, HIGH);
  }else{ // 蓝牙控制
    digitalWrite(IO_BLE_CTL, LOW);
  }
}

/* header header len cmd count time [id duty] */
void PC_BLE_CTL::PC_BLE_Task(LeArm_t* robot,Led_t* led,Buzzer_t* buzzer)
{
  unpack();
  if(unpack_successful)
  {
    unpack_successful = false;
    // Serial.print("unpack successful: ");
    // Serial.println(packet.cmd);
    switch(packet.cmd)
    {
    case 1:
      status = APP_OFFSET_READ;
      break;

    case 2:
      status = APP_OFFSET_DOWNLOAD;
      break;
    case 3:
      status = APP_FUNC_MULT_SERVO_MOVE;
      break;
    case 4:
      status = APP_COORDINATE_SET;
      break;
    case 6:
      status = APP_ACTION_GROUP_RUN;
      robot->reset();
      robot->action_group_reset();
      delay(900);
      break;
    case 7:
      // Serial.println("stop");
      status = APP_FULL_ACTION_STOP;
      break;
    case 8:
      status = APP_FULL_ACTION_ERASE;
      break;
    case 25:
      status = APP_ACTION_DOWNLOAD;
      break;
    default:
      status = APP_FUNC_NULL;
      break;
    }
  }

  switch (status)
  {
      case APP_OFFSET_READ:
          break;
      case APP_OFFSET_DOWNLOAD:
          break;
      case APP_FUNC_MULT_SERVO_MOVE:
          servos_count = packet.buffer[0];
          running_time = MERGE_HL(packet.buffer[2], packet.buffer[1]);
          for (uint8_t i = 0; i < servos_count; i++)
          {
              set_id = packet.buffer[3 + i * 3];
              set_duty = MERGE_HL(packet.buffer[5 + i * 3], packet.buffer[4 + i * 3]);
              robot->knot_run(set_id, set_duty, running_time);
          }
          status = APP_FUNC_NULL;
          break;

      case APP_COORDINATE_SET:
          set_id = packet.buffer[0];
          set_duty = MERGE_HL(packet.buffer[2], packet.buffer[1]);
          robot->knot_run(set_id, set_duty, running_time);

      case APP_ACTION_GROUP_RUN:
          packet.action_group_index = packet.buffer[0];
          packet.running_times = MERGE_HL(packet.buffer[2], packet.buffer[1]);
          if(robot->action_group_run(packet.action_group_index, packet.running_times))
          {
            // Serial.println("over");
            status = APP_FUNC_NULL;
          }
          break;

      case APP_FULL_ACTION_STOP:
          // Serial.println("stop action.");
          robot->action_group_stop();
          status = APP_FUNC_NULL;
          break;

      case APP_FULL_ACTION_ERASE:
          robot->action_group_erase();
          delay(5);
          packet_transmit(APP_FULL_ACTION_ERASE);
          status = APP_FUNC_NULL;
          break;

      case APP_ACTION_DOWNLOAD:
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
          packet_transmit(APP_ACTION_DOWNLOAD);
    // Serial.print("group index:");
    // Serial.print(packet.action_group_index);
    // Serial.print("sum:");
    // Serial.print(packet.action_frame_sum);
    // Serial.print("frame index:");
    // Serial.println(packet.action_frame_index);
          status = APP_FUNC_NULL;
          break;

      default:
          break;
  }
  delay(20);
}
