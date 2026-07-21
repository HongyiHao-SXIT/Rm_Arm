// #include "global.h"
// #include "Kinematics.h"
#include "./../../Robot_arm.hpp"
// #include "w25q64.h"

// KinematicsObjectTypeDef  kinematics;
// RobotArmHandleTypeDef robot_arm;

/* 用于保存已有的动作组中各自的总动作帧数量 */
uint8_t af_sum[ACTION_GROUP_MAX_NUM];	

static bool set_pitch_range(KinematicsObjectTypeDef* self, VectorObjectTypeDef* vector, float alpha1, float alpha2)
{
	float inc;
	self->vector.x = vector->x;
	self->vector.y = vector->y;
	self->vector.z = vector->z;
	inc = (alpha1 > alpha2) ? -1.0f : 1.0f;
	
	for (self->alpha = alpha1;
		 (inc > 0) ? (self->alpha < alpha2) : (self->alpha >= alpha2);
		 self->alpha += inc)
	{
		if (ikine(self) == K_OK)
		{
			return true;
		}
	}
	return false;
}

void LeArm_t::theta2servo(KinematicsObjectTypeDef* self, float time)
{
	float target_angle[4] = {0};
	target_angle[0] = 90.0f + self->knot[0].theta;
	target_angle[1] = 180.0f - self->knot[1].theta;
	target_angle[2] = 90.0f + self->knot[2].theta;
	target_angle[3] = 90.0f + self->knot[3].theta;
	for (uint8_t i = 0; i < 4; i++)
	{
    pwmservo_obj.set_angle(i, target_angle[i], time);
	}
}



/* 
 * 6号舵机 左+ 右-
 * 5号舵机 前+ 后-
 * 4号舵机 前- 后+
 * 3号舵机 前- 后+
 */
//bool result1_state, result2_state;
uint8_t LeArm_t::coordinate_set(float target_x,float target_y,float target_z,float pitch,float min_pitch,float max_pitch,uint32_t time)
{
	bool result1_state, result2_state;
	
	KinematicsObjectTypeDef kinematics_result1;
	KinematicsObjectTypeDef kinematics_result2;	
	VectorObjectTypeDef vector;

	vector.x = target_x;
	vector.y = target_y;
	vector.z = target_z;
	
	result1_state = set_pitch_range(&kinematics_result1, &vector, pitch, min_pitch);
	result2_state = set_pitch_range(&kinematics_result2, &vector, pitch, max_pitch);
	
	if (result1_state)
	{
		kinematics.alpha = kinematics_result1.alpha;
		kinematics.vector.x = kinematics_result1.vector.x;
		kinematics.vector.y = kinematics_result1.vector.y;
		kinematics.vector.z = kinematics_result1.vector.z;
		for (uint8_t i = 0; i< 4; i++)
		{
			kinematics.knot[i].theta = kinematics_result1.knot[i].theta;
		}
		
		if (result2_state)
		{
			if (fabs(kinematics_result2.alpha - pitch) < fabs(kinematics_result1.alpha - pitch))
			{
				kinematics.alpha = kinematics_result2.alpha;
				kinematics.vector.x = kinematics_result2.vector.x;
				kinematics.vector.y = kinematics_result2.vector.y;
				kinematics.vector.z = kinematics_result2.vector.z;
				for (uint8_t i = 0; i< 4; i++)
				{
					kinematics.knot[i].theta = kinematics_result2.knot[i].theta;
				}			
			}
		}
	}
	else
	{
		if (result2_state)
		{
			kinematics.alpha = kinematics_result2.alpha;
			kinematics.vector.x = kinematics_result2.vector.x;
			kinematics.vector.y = kinematics_result2.vector.y;
			kinematics.vector.z = kinematics_result2.vector.z;
			for (uint8_t i = 0; i< 4; i++)
			{
				kinematics.knot[i].theta = kinematics_result2.knot[i].theta;
			}
		}
		else
		{
			return false;
		}
	}
	result1_state = 0;
	result2_state = 0;
	theta2servo(&kinematics, time);
	return true;
}

/*
 * 2号舵机 右转- 左转+
 */
void LeArm_t::roll_set(float rotation_angle, uint32_t rotation_angle_time)
{
	float target_rotation_angle;
	rotation_angle = rotation_angle > MAX_ROTATION_ANGLE ? MAX_ROTATION_ANGLE : \
					(rotation_angle < MIN_ROTATION_ANGLE ? MIN_ROTATION_ANGLE : rotation_angle);
	
	target_rotation_angle = 90.0f - rotation_angle;
	// pwm_servo_angle_set(&pwm_servos[4], target_rotation_angle, rotation_angle_time);
  pwmservo_obj.set_angle(4, target_rotation_angle, rotation_angle_time);
}

/*
 * 1号舵机 张开- 闭合+
 */
void LeArm_t::claw_set(float open_angle, uint32_t open_angle_time)
{
	float target_open_angle;
	
	open_angle = open_angle > MAX_OPEN_ANGLE ? MAX_OPEN_ANGLE : \
				(open_angle < MIN_OPEN_ANGLE ? MIN_OPEN_ANGLE : open_angle);
	target_open_angle = 90.0f - open_angle;
	// pwm_servo_angle_set(&pwm_servos[5], target_open_angle, open_angle_time);
  pwmservo_obj.set_angle(5, target_open_angle, open_angle_time);
}

void LeArm_t::knot_run(uint8_t group_id, int target_duty, uint32_t time)
{
	switch(group_id)
	{
		case 1:
			if(target_duty > 1600)
			{
				target_duty = 1600;
			}
      pwmservo_obj.set_duty(5, target_duty, time);
			break;
		case 2:
      pwmservo_obj.set_duty(4, target_duty, time);
			break;
		case 3:
      pwmservo_obj.set_duty(3, target_duty, time);
			break;
		case 4:
      pwmservo_obj.set_duty(2, target_duty, time);
			break;
		case 5:
      pwmservo_obj.set_duty(1, target_duty, time);
			break;
		case 6:
      pwmservo_obj.set_duty(0, target_duty, time);
			break;
		default:
			break;
	}
}

void LeArm_t::knot_stop(uint8_t group_id)
{
	switch(group_id)
	{
		case 1:
      pwmservo_obj.stop(5);
			break;
		case 2:
      pwmservo_obj.stop(4);
			break;
		case 3:
      pwmservo_obj.stop(3);
			break;
		case 4:
      pwmservo_obj.stop(2);
			break;
		case 5:
      pwmservo_obj.stop(1);
			break;
		case 6:
      pwmservo_obj.stop(0);
			break;
		default:
			break;
	}
}

bool LeArm_t::knot_finish(uint8_t id)
{
	return pwmservo_obj.is_ready(id);
}

void LeArm_t::action_group_erase()
{
	/* 将所有动作组的动作帧数量设置为0，即代表将所有动作组擦除 */
    memset(af_sum, 0, sizeof(af_sum));
    flash_obj.erase_sector(ACTION_FRAME_SUM_BASE_ADDRESS);
    flash_obj.write(ACTION_FRAME_SUM_BASE_ADDRESS,(uint8_t*)af_sum,sizeof(af_sum));
	delay(5);
	// Serial.print("erase:");
	// flash_obj.read(ACTION_FRAME_SUM_BASE_ADDRESS,(uint8_t*)af_sum,sizeof(af_sum));
}

// uint8_t read[ACTION_FRAME_SIZE] = {0};
void LeArm_t::action_group_init()
{
	uint8_t read_logo[9] = {0};
	const uint8_t logo[] = "Hiwonder";
    flash_obj.read(LOGO_BASE_ADDRESS, read_logo, sizeof(read_logo));
    // Serial.print("read 1 logo: ");
    // for(int i = 0 ; i < 9 ; i++)
    // {
    //     Serial.print(read_logo[i] , HEX);
    //     Serial.print("|");
    // }
    // Serial.println("");

	for (uint8_t i = 0; i < sizeof(read_logo); i++)
	{
		if (read_logo[i] != logo[i])
		{
			flash_obj.erase_sector(LOGO_BASE_ADDRESS);
			// delay(100);
			flash_obj.write(LOGO_BASE_ADDRESS, (uint8_t*)logo, sizeof(logo));
            // delay(100);
            action_group_erase();
			// delay(100);
            flash_obj.read(LOGO_BASE_ADDRESS, read_logo, sizeof(read_logo));
            // Serial.print("read 2 logo: ");
            // for(int i = 0 ; i < 9 ; i++)
            // {
            //     Serial.print(read_logo[i] , HEX);
            //     Serial.print("|");
            // }
            // Serial.println("");
            return;
		}
	}
    // Serial.println("group init ok.");
}

/* 一个的动作帧的数组内容
 * 控制的舵机数量：frame[0]
 * 运行时间：frame[1] + frame[2] << 8
 * 舵机id：frame[3 + i * 3]:
 * 舵机脉宽：frame[4 + i * 3] + frame[5 + i * 3] << 8
 * 0-动作帧运行失败 1-动作帧运行完成     0100 1101 0001
 */
uint8_t frame[ACTION_FRAME_SIZE];
uint8_t LeArm_t::action_frame_run(uint8_t action_group_index, uint8_t frame_index)
{
	uint8_t set_id;
	uint8_t control_servos_sum;

	
	uint16_t set_duty_num;
	uint32_t ag_addr_offset;
	uint32_t af_addr_offset;

	switch(robot_arm.action_group.frame.status)
	{
		case ACTION_FRAME_START:
			ag_addr_offset = action_group_index * ACTION_GROUP_SIZE;
			af_addr_offset = frame_index * ACTION_FRAME_SIZE;
			flash_obj.read(ACTION_GROUP_BASE_ADDRESS + ag_addr_offset + af_addr_offset,
						frame, sizeof(frame));
			control_servos_sum = frame[0];

			if (control_servos_sum > MAX_SERVOS_NUM)
			{
				return ACTION_FRAME_START;
			}
			robot_arm.action_group.frame.time = MERGE_HL(frame[2], frame[1]);

			for (uint8_t i = 0; i < control_servos_sum; i++)
			{
				set_id = frame[3 + i * 3];
				set_duty_num = (uint16_t)MERGE_HL(frame[5 + i * 3], frame[4 + i * 3]);
				knot_run(set_id, (int)set_duty_num, robot_arm.action_group.frame.time);
			}

			robot_arm.action_group.frame.status = ACTION_FRAME_RUNNING;
			break;
		
		case ACTION_FRAME_RUNNING:
			delay(100);
		    /* 取一个舵机来判断是否来到达指定位置 */
			if (pwmservo_obj.is_ready(0) && pwmservo_obj.is_ready(1) && pwmservo_obj.is_ready(2)
          && pwmservo_obj.is_ready(3) && pwmservo_obj.is_ready(4) && pwmservo_obj.is_ready(5))
			{
				robot_arm.action_group.frame.index++;
				robot_arm.action_group.frame.status = ACTION_FRAME_IDLE;
			}
			break;
		
		case ACTION_FRAME_IDLE:
			break;
		
		default:
			break;
	}

	return robot_arm.action_group.frame.status;
}

bool LeArm_t::action_group_run(uint8_t action_group_index, uint8_t running_times)
{
//	while(robot_arm.action_group.status != ACTION_GROUP_IDLE)
//	{
	static bool state = true;
		switch (robot_arm.action_group.status)
		{
			case ACTION_GROUP_START:
				state = false;
				robot_arm.action_group.running_times = running_times;
				robot_arm.action_group.index = action_group_index;
				robot_arm.action_group.frame.index = 0;
				flash_obj.read(ACTION_FRAME_SUM_BASE_ADDRESS + action_group_index,
							&robot_arm.action_group.sum, 1);
				/* 如果该动作组的动作帧数量大于0，则说明已经下载过动作 */
				robot_arm.action_group.status = robot_arm.action_group.sum > 0 ? \
												ACTION_GROUP_RUNNING : ACTION_GROUP_IDLE;
				// if(ACTION_GROUP_IDLE == robot_arm.action_group.status)
				// {
				// 	Serial.println("group null.");
				// }
				break;

			case ACTION_GROUP_RUNNING:
				// Serial.print("group_run:");
				// Serial.println(action_group_index);
				if(action_frame_run(robot_arm.action_group.index, robot_arm.action_group.frame.index) == ACTION_FRAME_IDLE)
				{
					if (robot_arm.action_group.frame.index == robot_arm.action_group.sum)
					{
						robot_arm.action_group.status = ACTION_GROUP_END_PERIOD;
						robot_arm.action_group.frame.index = 0;
					}
					else
					{
						robot_arm.action_group.frame.status = ACTION_FRAME_START;
					}
				}
				/* code */
				break;

			case ACTION_GROUP_END_PERIOD:
//				if(robot_arm.action_group.running_times <= 1)
//				{
					robot_arm.action_group.status = ACTION_GROUP_IDLE;
//				}
//				else
//				{
//					--robot_arm.action_group.running_times;
//					robot_arm.action_group.status = ACTION_GROUP_RUNNING;
//				}
//				break;

			case ACTION_GROUP_IDLE:
				state = true;
				break;
			
			default:
				break;
		}
		return state;
//	}
}

void LeArm_t::action_run(uint8_t action_group_index, uint8_t repeat_times)
{
	while(!action_group_run(action_group_index , repeat_times));
	action_group_reset();
}

void LeArm_t::action_group_reset()
{
	robot_arm.action_group.status = ACTION_GROUP_START;
}

void LeArm_t::action_group_stop()
{
	for (uint8_t i = 0; i < MAX_SERVOS_NUM; i++)
	{
		knot_stop(i);
	}
	robot_arm.action_group.status = ACTION_GROUP_IDLE;
	robot_arm.action_group.frame.status = ACTION_FRAME_START;
}

uint8_t write_frame[8][ACTION_FRAME_SIZE] = {0};
uint8_t read_frame[8][ACTION_FRAME_SIZE] = {0};
int LeArm_t::action_group_save(uint8_t action_group_index, uint8_t frame_num,uint8_t frame_index,uint8_t* pdata,uint16_t size)
{
	uint32_t ag_addr_offset;
	uint32_t af_addr_offset;
	
	robot_arm.action_group.index = action_group_index;
	robot_arm.action_group.frame.index = frame_index;
	ag_addr_offset = action_group_index * ACTION_GROUP_SIZE;
	af_addr_offset = frame_index * ACTION_FRAME_SIZE;
	
	/* 如果写入对应动作组的第一帧，则需要先擦除该动作组的所有内容 */
	if (frame_index == 0)
	{
		/* 一个动作组占8KB，擦除一个扇区是4KB，则需要擦除2次 */
		for (uint8_t i = 0; i < 2; i++)
		{
			flash_obj.erase_sector(ACTION_GROUP_BASE_ADDRESS + ag_addr_offset + (i * 4096));
		}
	}
	flash_obj.write(ACTION_GROUP_BASE_ADDRESS + ag_addr_offset + af_addr_offset, (uint8_t*)pdata, size);
	delay(5);
	memcpy(write_frame[frame_index], pdata, size);
	// flash_obj.read(ACTION_GROUP_BASE_ADDRESS + ag_addr_offset + af_addr_offset, read_frame[frame_index], sizeof(read_frame[frame_index]));
	if ((robot_arm.action_group.frame.index + 1) == frame_num)
	{
		/* 如果写入的是最后一帧，此时需要更新一下flash中对应动作组的动作帧总数 */
		flash_obj.read(ACTION_FRAME_SUM_BASE_ADDRESS,af_sum, sizeof(af_sum));
		af_sum[robot_arm.action_group.index] = frame_num;
		flash_obj.erase_sector(ACTION_FRAME_SUM_BASE_ADDRESS);
		flash_obj.write(ACTION_FRAME_SUM_BASE_ADDRESS,(uint8_t*)af_sum,sizeof(af_sum));
		delay(5);
		return 1;
	}
	return 0;
}

void LeArm_t::init(void)
{
    flash_obj.init();
    pwmservo_obj.init();
	kinematics_init(&kinematics);
	memset(&robot_arm, 0, sizeof(RobotArmHandleTypeDef));
	action_group_init();
}

void LeArm_t::reset()
{
	// Serial.println("reset.");
	knot_run(6, SERVO6_RESET_DUTY, 800);
	knot_run(5, SERVO5_RESET_DUTY, 800);
	knot_run(4, SERVO4_RESET_DUTY, 800);
	knot_run(3, SERVO3_RESET_DUTY, 800);
	knot_run(2, SERVO2_RESET_DUTY, 800);
	knot_run(1, SERVO1_RESET_DUTY, 800);
}

void LeArm_t::servos_offset_read(void)
{
	uint8_t i;
	uint8_t _data[12] = {0};
	flash_obj.read(SERVOS_OFFSET_VALUE, _data, sizeof(_data));
	for(i = 0; i < 6; i++)
	{
		switch(_data[i * 2])
		{
			case 1:
				pwmservo_obj.set_offset(5, (int)_data[(i * 2) + 1]);
				// pwm_servos[5].offset = (int)_data[(i * 2) + 1];
				break;
			
			case 2:
				pwmservo_obj.set_offset(4, (int)_data[(i * 2) + 1]);
				// pwm_servos[4].offset = (int)_data[(i * 2) + 1];
				break;
			
			case 3:
				pwmservo_obj.set_offset(3, (int)_data[(i * 2) + 1]);
				// pwm_servos[3].offset = (int)_data[(i * 2) + 1];
				break;
			
			case 4:
				pwmservo_obj.set_offset(2, (int)_data[(i * 2) + 1]);
				// pwm_servos[2].offset = (int)_data[(i * 2) + 1];
				break;
			
			case 5:
				pwmservo_obj.set_offset(1, (int)_data[(i * 2) + 1]);
				// pwm_servos[1].offset = (int)_data[(i * 2) + 1];
				break;
			
			case 6:
				pwmservo_obj.set_offset(0, (int)_data[(i * 2) + 1]);
				// pwm_servos[0].offset = (int)_data[(i * 2) + 1];
				break;			
		}
	}
}

void LeArm_t::servos_offset_erase(void)
{
	uint8_t _data[6] = {0};
	flash_obj.erase_sector(SERVOS_OFFSET_VALUE);
	flash_obj.write(SERVOS_OFFSET_VALUE, (uint8_t*)_data, sizeof(_data));
}

void LeArm_t::servos_offset_save(uint8_t* _pdata, uint16_t _size)
{
	flash_obj.write(SERVOS_OFFSET_VALUE, (uint8_t*)_pdata, _size);
}
