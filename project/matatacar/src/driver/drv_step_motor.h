#ifndef __DRV_STEP_MOTOR_H__
#define __DRV_STEP_MOTOR_H__

#include "esp_err.h"
#include "freertos/semphr.h"
#include "esp_task.h" 
#include "soc/ledc_reg.h"

// GPIO configure
#define MOTOR_LEFT_SLEEP_PIN     (38)
#define MOTOR_RIGHT_SLEEP_PIN    (38)
#define MOTOR_LEFT_INA_PIN       (12)
#define MOTOR_LEFT_INB_PIN       (13)
#define MOTOR_RIGHT_INA_PIN      (14)
#define MOTOR_RIGHT_INB_PIN      (15)
#define SLEEP_PIN_SEL            ((1ULL << MOTOR_LEFT_SLEEP_PIN) | (1ULL << MOTOR_RIGHT_SLEEP_PIN))

#define MOTOR_LEFT_FAULT_PIN      (-1)
#define MOTOR_RIGHT_FAULT_PIN     (-1)
#define FAULT_PIN_SEL            ((1ULL << MOTOR_LEFT_FAULT_PIN) | (1ULL << MOTOR_RIGHT_FAULT_PIN))

// Timer configure
#define MOTOR_LEFT_TIME          LEDC_TIMER_0
#define MOTOR_LEFT_SPEED_MODE    LEDC_LOW_SPEED_MODE
#define MOTOR_LEFT_INA_CHANNEL   LEDC_CHANNEL_0
#define MOTOR_LEFT_INB_CHANNEL   LEDC_CHANNEL_1

#define MOTOR_RIGHT_TIME         LEDC_TIMER_1
#define MOTOR_RIGHT_SPEED_MODE   LEDC_LOW_SPEED_MODE
#define MOTOR_RIGHT_INA_CHANNEL  LEDC_CHANNEL_2
#define MOTOR_RIGHT_INB_CHANNEL  LEDC_CHANNEL_3

#define MOTOR_CHANNEL_NUM        (4)

// 64 细分, 一个循环四个信号
#define STEP_SUBDIVISION         32
#define MICRO_STEP               (STEP_SUBDIVISION * 4)
#define STEP_POS_MAX             (MICRO_STEP * 100)

#define DEFAULT_MIN_SPEED        (200 * STEP_SUBDIVISION)
#define DEFALUT_MAX_SPEED        (1050 * STEP_SUBDIVISION)
#define DEFALUT_ACCELERATION     (5 * DEFALUT_MAX_SPEED)    //200ms 加速到最快


// Task
#define STEP_MOTOR_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 2)
#define STEP_MOTOR_TASK_STACK_SIZE      (4 * 1024)

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

typedef enum
{
    MOTOR_LEFT  = 0,
    MOTOR_RIGHT = 1,
    MOTOR_MAX   = 2,
}motor_configure_type;

typedef enum
{
    DIR_CW =  0,
    DIR_CCW = 1,
}step_motor_dir_type;

typedef enum
{
    STOP_MOVE = 0,
    SPEED_MOVE =  1,
    POS_MOVE = 2,
}step_motor_motion_type;

typedef struct
{
    step_motor_dir_type    dir;
    step_motor_motion_type motion_status;
    volatile int32_t       step_pos;
    uint32_t               current_spwm_freq;
    volatile long          current_pos;
    long                   target_pos;
    volatile int32_t       speed;
    int32_t                target_speed;
    int32_t                max_speed;

    /// The acceleration to use to accelerate or decelerate the motor in steps
    /// per second per second. Must be > 0
    int32_t                acceleration;

    /// The step counter for speed calculations
    int32_t                _n;

    /// Initial step size in microseconds
    int32_t               _c0;

    /// Last step size in microseconds
    int32_t               _cn;

    /// Min step size in microseconds based on maxSpeed
    int32_t               _cmin; // at max speed
}step_motor_data_type;

typedef struct
{
	step_motor_data_type motor_data[MOTOR_MAX];
    SemaphoreHandle_t motion_task_init_mutex;
}motion_data_type;

extern motion_data_type motion_data;

extern void step_motor_init(void);
extern void step_sleep(motor_configure_type motor, bool enable);
extern void step_start(motor_configure_type motor);
extern void compute_new_speed(motor_configure_type motor);
extern void motor_set_move_speed_max(int32_t max_speed);
extern void set_max_speed(motor_configure_type motor, int32_t speed);
extern void set_acceleration(motor_configure_type motor, int32_t acceleration);
extern long get_target_position(motor_configure_type motor);
extern long get_current_position(motor_configure_type motor);
extern void set_current_position(motor_configure_type motor, long position);
extern void motor_move_to(motor_configure_type motor, long absolute, bool sync);
extern void motor_move(motor_configure_type motor, long relative, bool sync);
extern void motor_set_speed(motor_configure_type motor, int32_t run_speed);
extern int32_t motor_get_speed(motor_configure_type motor);
extern void motor_run_speed(int32_t left_run_speed, int32_t right_run_speed);
extern void motor_stop(motor_configure_type motor);
extern void motor_run(void);
extern void step_motor_task(void *pvParameter);

#endif /* __DRV_STEP_MOTOR_H__ */ 
