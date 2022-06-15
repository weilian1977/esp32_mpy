#ifndef __MPY_MOTION_H__
#define __MPY_MOTION_H__

#include "esp_err.h"

#include "mt_module_config.h"


// 64 细分, 一个循环四个信号
#define STEP_SUBDIVISION         32
#define MICRO_STEP               (STEP_SUBDIVISION * 4)
#define STEP_POS_MAX             (MICRO_STEP * 100)

#define DEFAULT_MIN_SPEED        (200 * STEP_SUBDIVISION)
#define DEFALUT_MAX_SPEED        (1050 * STEP_SUBDIVISION)
#define DEFALUT_ACCELERATION     (5 * DEFALUT_MAX_SPEED)    //200ms 加速到最快

#define WHEEL_DIAMETER_VALUE                  45.6f
#define WHEEL_SPACING_VALUE                   62.0f
#define MOTOR_REDUCTION_RATIO                 45.28f
#define STEP_ANGLE                            18
#define PI                                    3.1416f

#define MM_TO_PULSE                           (((360 / STEP_ANGLE) * MOTOR_REDUCTION_RATIO * STEP_SUBDIVISION) / (WHEEL_DIAMETER_VALUE * PI))
#define STEP_TO_PULSE                         (100 * MM_TO_PULSE)
#define DEGREE_TO_PULSE                       (WHEEL_SPACING_VALUE * PI * MM_TO_PULSE / 360.0f)                  

extern const mp_obj_type_t mpy_motion_type;

#endif /* __MPY_MOTION_H__ */ 
