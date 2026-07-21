#ifndef __QMI8658_H__
#define __QMI8658_H__

#include <Arduino.h>
#include "IIC.hpp"

#define QMI8658_ADDR	0x68

#define ACCEL_XOUT_H	0x3B
#define ACCEL_XOUT_L	0x3C
#define ACCEL_YOUT_H	0x3D
#define ACCEL_YOUT_L	0x3E
#define ACCEL_ZOUT_H	0x3F
#define ACCEL_ZOUT_L	0x40

#define TEMP_OUT_H		0x41
#define TEMP_OUT_L		0x42

#define GYRO_XOUT_H		0x43
#define GYRO_XOUT_L		0x44
#define GYRO_YOUT_H		0x45
#define GYRO_YOUT_L		0x46
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48

#define EULER_XOUT_H	0x49
#define EULER_XOUT_L	0x4A
#define EULER_YOUT_H	0x4B
#define EULER_YOUT_L	0x4C
#define EULER_ZOUT_H	0x4D
#define EULER_ZOUT_L	0x4E

typedef struct
{
	float x;
	float y;
	float z;
}ParametersObejctTypedDef;

#define MPU_ADDR  0x68

class IMU {
  private:
    IIC* iic;
	uint16_t dev_addr;
	
	ParametersObejctTypedDef acc;
	ParametersObejctTypedDef gyro;
  public:
    IMU();
    void init(IIC* iic_obj);
	void get_accelerometer(float* acc);
    void get_gyro(float* gyro);
	void get_euler(float* euler);
};

#endif
