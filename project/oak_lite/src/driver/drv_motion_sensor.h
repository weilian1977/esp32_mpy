#ifndef __DRV_MOTION_SENSOR_H__
#define __DRV_MOTION_SENSOR_H__

float invSqrt(float x);
void IMU_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) ;
void IMU_GetQuater(void);
void IMU_GetYawPitchRoll(float *Angles) ;
void reset_pos(void);
#endif
