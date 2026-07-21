#ifndef WONDERMV_H
#define WONDERMV_H

#include <Arduino.h>
#include "IIC.hpp"

#define WONDERMV_ADDR  0x32
#define COLOR_REG  	   0x00
#define FACE_REG  	   0x10
#define TAG_REG  	   0x20
#define OBJECT_REG     0x30

typedef struct{
  uint8_t id;
  uint16_t w;
	uint16_t h;
	uint16_t x;
	uint16_t y;
}MV_RESULT_ST;

class WonderMV {
  private:
    IIC* iic;
  public:
    WonderMV();
    void init(IIC* iic_obj);
    void get_result(uint8_t reg , MV_RESULT_ST* result);
};

#endif //WONDERMV_H
