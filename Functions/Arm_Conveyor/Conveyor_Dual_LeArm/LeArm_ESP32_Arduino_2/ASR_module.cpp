#include "ASR_module.hpp"

ASR_MOUDLE::ASR_MOUDLE(void)
{
  send[0] = 0;
  send[1] = 0;
  iic = nullptr;
}

void ASR_MOUDLE::init(IIC* iic_obj)
{
  iic = iic_obj;
}

uint8_t ASR_MOUDLE::rec_recognition(void)
{
  if(iic != nullptr){
    uint8_t result = 0;
    iic->wireReadDataArray(I2C_ADDR , ASR_RESULT_ADDR ,&result,1);
    return result;
  }
  return 0;
}

void ASR_MOUDLE::speak(uint8_t cmd , uint8_t id)
{
  if(iic != nullptr){
    if(cmd == 0xFF || cmd == 0x00)
    {
      send[0] = cmd;
      send[1] = id;
      iic->wireWriteDataArray(I2C_ADDR , ASR_SPEAK_ADDR , send , 2);
    }
  }
}
