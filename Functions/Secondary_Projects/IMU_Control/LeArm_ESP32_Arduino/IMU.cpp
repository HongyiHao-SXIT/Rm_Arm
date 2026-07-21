#include "IMU.hpp"

#define BYTE_TO_HW(A, B) ((((uint16_t)(A)) << 8) | (uint8_t)(B))


IMU::IMU()
{
  iic = nullptr;
}

void IMU::init(IIC* iic_obj)
{
  iic = iic_obj;
  dev_addr = QMI8658_ADDR;
}

void IMU::get_accelerometer(float* acc)
{
	static uint8_t rec_data[6];
	
	for(uint8_t i = 0; i < 6; i++)
	{
		iic->wireReadDataArray(dev_addr, ACCEL_XOUT_H + i, &rec_data[i], 1);
		delay(1);
	}
	acc[0] = (float)((int16_t)BYTE_TO_HW(rec_data[0], rec_data[1])) / 100.0f;
	acc[1] = (float)((int16_t)BYTE_TO_HW(rec_data[2], rec_data[3])) / 100.0f;
	acc[2] = (float)((int16_t)BYTE_TO_HW(rec_data[4], rec_data[5])) / 100.0f;
}

void IMU::get_gyro(float* gyro)
{
	static uint8_t rec_data[6];
	
	for(uint8_t i = 0; i < 6; i++)
	{
		iic->wireReadDataArray(dev_addr, GYRO_XOUT_H + i, &rec_data[i], 1);
		delay(1);
	}
	gyro[0] = (float)((int16_t)BYTE_TO_HW(rec_data[0], rec_data[1])) / 100.0f;
	gyro[1] = (float)((int16_t)BYTE_TO_HW(rec_data[2], rec_data[3])) / 100.0f;
	gyro[2] = (float)((int16_t)BYTE_TO_HW(rec_data[4], rec_data[5])) / 100.0f;
}

void IMU::get_euler(float* euler)
{
	static uint8_t rec_data[6];
	
	for(uint8_t i = 0; i < 6; i++)
	{
		iic->wireReadDataArray(dev_addr, EULER_XOUT_H + i, &rec_data[i], 1);
		delay(1);
	}
	euler[0] = (float)((int16_t)BYTE_TO_HW(rec_data[0], rec_data[1])) / 100.0f;
	euler[1] = (float)((int16_t)BYTE_TO_HW(rec_data[2], rec_data[3])) / 100.0f;
	euler[2] = (float)((int16_t)BYTE_TO_HW(rec_data[4], rec_data[5])) / 100.0f;
}
