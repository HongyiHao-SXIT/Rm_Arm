#include <Wire.h>
#include "WonderMV.hpp"
#include "Config.h"

#define BYTE_TO_HW(A, B) ((((uint16_t)(A)) << 8) | (uint8_t)(B))

WonderMV::WonderMV()
{
  iic = nullptr;
}

void WonderMV::init(IIC* iic_obj)
{
  iic = iic_obj;
}

void WonderMV::get_result(uint8_t reg , MV_RESULT_ST* result)
{
  if(iic != nullptr){
    uint8_t rec[9];
    iic->wireReadDataArray(WONDERMV_ADDR , reg , rec , sizeof(rec));
    result->id = rec[0];
    result->x = BYTE_TO_HW(rec[2], rec[1]);
    result->y = BYTE_TO_HW(rec[4], rec[3]);
    result->w = BYTE_TO_HW(rec[6], rec[5]);
    result->h = BYTE_TO_HW(rec[8], rec[7]);
  }
}
