#ifndef __ASR_MODULE_H
#define __ASR_MODULE_H

#include "IIC.hpp"

#define I2C_ADDR		0x34
//识别结果存放处，通过不断读取此地址的值判断是否识别到语音，不同的值对应不同的语音
#define ASR_RESULT_ADDR   0x64
#define ASR_SPEAK_ADDR    0x6E

#define ASR_CMDMAND    0x00
#define ASR_ANNOUNCER  0xFF

class ASR_MOUDLE
{
  public:
    ASR_MOUDLE(void);
    void init(IIC* iic_obj);
    uint8_t rec_recognition(void);
    void speak(uint8_t cmd , uint8_t id);

  private:
    IIC* iic;
    uint8_t send[2];
};

#endif //__ASR_MODULE_H

