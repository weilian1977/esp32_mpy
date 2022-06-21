#include <stdio.h>
#include <string.h>
#include <math.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "drv_step_motor.h"
#include "mpy_motion.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "mpy motion";

#if MODULE_MOTION_ENABLE
/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/
typedef struct {
    mp_obj_base_t base;
}mpy_motion_obj_t;

STATIC void mpy_motion_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_printf(print, "%s\r\n", TAG);
    mp_printf(print, "motion print\r\n");
};

STATIC  mpy_motion_obj_t mpy_motion_obj = { .base = {&mpy_motion_type} };

STATIC mp_obj_t mpy_motion_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    // setup the object
    mpy_motion_obj_t *self = &mpy_motion_obj;
    self->base.type = &mpy_motion_type;
    return self;
}

STATIC mp_obj_t mpy_forward(mp_obj_t self_in, mp_obj_t distance, mp_obj_t sync)
{
    int32_t distance_value = mp_obj_get_int(distance);
    long pulse = (long)(MM_TO_PULSE * distance_value);
    bool sync_flag = mp_obj_get_int(sync);
    if(sync_flag)
    {
        motor_move(MOTOR_LEFT, -pulse, false);
        motor_move(MOTOR_RIGHT, pulse, true);
    }
    else
    {
        motor_move(MOTOR_LEFT, -pulse, false);
        motor_move(MOTOR_RIGHT, pulse, false);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(forward_obj, mpy_forward);

STATIC mp_obj_t mpy_backward(mp_obj_t self_in, mp_obj_t distance, mp_obj_t sync)
{
    int32_t distance_value = mp_obj_get_int(distance);
    long pulse = (long)(MM_TO_PULSE * distance_value);
    bool sync_flag = mp_obj_get_int(sync);
    if(sync_flag)
    {
        motor_move(MOTOR_LEFT, pulse, false);
        motor_move(MOTOR_RIGHT, -pulse, true);
    }
    else
    {
        motor_move(MOTOR_LEFT, pulse, false);
        motor_move(MOTOR_RIGHT, -pulse, false);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(backward_obj, mpy_backward);

STATIC mp_obj_t mpy_turn_left(mp_obj_t self_in, mp_obj_t angle, mp_obj_t sync)
{
    int16_t angle_value = mp_obj_get_int(angle);
    long pulse = (long)(DEGREE_TO_PULSE * angle_value);
    bool sync_flag = mp_obj_get_int(sync);
    if(sync_flag)
    {
        motor_move(MOTOR_LEFT, pulse, false);
        motor_move(MOTOR_RIGHT, pulse, true);
    }
    else
    {
        motor_move(MOTOR_LEFT, pulse, false);
        motor_move(MOTOR_RIGHT, pulse, false);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(turn_left_obj, mpy_turn_left);

STATIC mp_obj_t mpy_turn_right(mp_obj_t self_in, mp_obj_t angle, mp_obj_t sync)
{
    int16_t angle_value = mp_obj_get_int(angle);
    long pulse = (long)(DEGREE_TO_PULSE * angle_value);
    bool sync_flag = mp_obj_get_int(sync);
    if(sync_flag)
    {
        motor_move(MOTOR_LEFT, -pulse, false);
        motor_move(MOTOR_RIGHT, -pulse, true);
    }
    else
    {
        motor_move(MOTOR_LEFT, -pulse, false);
        motor_move(MOTOR_RIGHT, -pulse, false);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(turn_right_obj, mpy_turn_right);

STATIC mp_obj_t mpy_move_position(mp_obj_t self_in, mp_obj_t position, mp_obj_t sync)
{
    int16_t position_value = mp_obj_get_int(position);
    long pulse = (long)(MM_TO_PULSE * position_value);
    bool sync_flag = mp_obj_get_int(sync);
    if(sync_flag)
    {
        motor_move(MOTOR_LEFT, -pulse, false);
        motor_move(MOTOR_RIGHT, pulse, true);
    }
    else
    {
        motor_move(MOTOR_LEFT, -pulse, false);
        motor_move(MOTOR_RIGHT, pulse, false);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(move_position_obj, mpy_move_position);

STATIC mp_obj_t mpy_move_angle(mp_obj_t self_in, mp_obj_t angle, mp_obj_t sync)
{
    int16_t angle_value = mp_obj_get_int(angle);
    long pulse = (long)(DEGREE_TO_PULSE * angle_value);
    bool sync_flag = mp_obj_get_int(sync);
    if(sync_flag)
    {
        motor_move(MOTOR_LEFT, -pulse, false);
        motor_move(MOTOR_RIGHT, -pulse, true);
    }
    else
    {
        motor_move(MOTOR_LEFT, -pulse, false);
        motor_move(MOTOR_RIGHT, -pulse, false);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(move_angle_obj, mpy_move_angle);

STATIC mp_obj_t mpy_move_speed(mp_obj_t self_in, mp_obj_t left_speed, mp_obj_t right_speed)
{
    int16_t left_speed_value = 0;
    int16_t right_speed_value = 0;
    long left_pulse_speed = 0;
    long right_pulse_speed = 0;
    if(MP_OBJ_IS_SMALL_INT(left_speed))
    {
        left_speed_value = mp_obj_get_int(left_speed);
        left_pulse_speed = -(long)(MM_TO_PULSE * left_speed_value);
    }
    else if(MP_OBJ_IS_QSTR(left_speed))
    {
        const char *left_str = mp_obj_str_get_str(left_speed);
        // mp_printf("left_speed is string (%s)\r\n", test_str);
        if(strcmp("unchanged", left_str) == 0)
        {
            left_pulse_speed = motor_get_speed(MOTOR_LEFT);
        }
        else if(strcmp("stop", left_str) == 0)
        {
            left_pulse_speed = 0;
        }
    }
    if(MP_OBJ_IS_SMALL_INT(right_speed))
    {
        right_speed_value = mp_obj_get_int(right_speed);
        right_pulse_speed = (long)(MM_TO_PULSE * right_speed_value);
    }
    else if(MP_OBJ_IS_QSTR(right_speed))
    {
        const char *right_str = mp_obj_str_get_str(right_speed);
        // mp_printf("left_speed is string (%s)\r\n", test_str);
        if(strcmp("unchanged", right_str) == 0)
        {
            right_pulse_speed = motor_get_speed(MOTOR_RIGHT);
        }
        else if(strcmp("stop", right_str) == 0)
        {
            right_pulse_speed = 0;
        }
    }
    motor_run_speed(left_pulse_speed, right_pulse_speed, false);

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(move_speed_obj, mpy_move_speed);

STATIC mp_obj_t mpy_motor_speed(mp_obj_t self_in, mp_obj_t motor, mp_obj_t speed)
{
    int16_t motor_value = mp_obj_get_int(motor);
    int16_t speed_value = mp_obj_get_int(speed);
    long pulse_speed = (long)(MM_TO_PULSE * speed_value);
    if(motor_value == MOTOR_MAX)
    {
        motor_set_speed(MOTOR_LEFT, pulse_speed, false);
        motor_set_speed(MOTOR_RIGHT, pulse_speed, false);
    }
    else if(motor_value == MOTOR_LEFT)
    {
        motor_set_speed(MOTOR_LEFT, pulse_speed, false);
    }
    else if(motor_value == MOTOR_RIGHT)
    {
        motor_set_speed(MOTOR_RIGHT, pulse_speed, false);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(motor_speed_obj, mpy_motor_speed);

STATIC mp_obj_t mpy_motor_pwm(mp_obj_t self_in, mp_obj_t motor, mp_obj_t pwm)
{
    int16_t motor_value = mp_obj_get_int(motor);
    int16_t pwm_value = mp_obj_get_int(pwm);
    long pulse_speed = (long)(DEFALUT_MAX_SPEED * pwm_value / 100);
    if(motor_value == MOTOR_MAX)
    {
        motor_set_speed(MOTOR_LEFT, pulse_speed, true);
        motor_set_speed(MOTOR_RIGHT, pulse_speed, true);
    }
    else if(motor_value == MOTOR_LEFT)
    {
        motor_set_speed(MOTOR_LEFT, pulse_speed, true);
    }
    else if(motor_value == MOTOR_RIGHT)
    {
        motor_set_speed(MOTOR_RIGHT, pulse_speed, true);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(motor_pwm_obj, mpy_motor_pwm);

STATIC mp_obj_t mpy_stop(mp_obj_t self_in, mp_obj_t motor)
{
    int16_t motor_value = mp_obj_get_int(motor);
    if(motor_value == MOTOR_MAX)
    {
        motor_stop(MOTOR_LEFT);
        motor_stop(MOTOR_RIGHT);
        set_current_position(MOTOR_LEFT, 0);
        set_current_position(MOTOR_RIGHT, 0);
    }
    else if(motor_value == MOTOR_LEFT)
    {
        motor_stop(MOTOR_LEFT);
        set_current_position(MOTOR_LEFT, 0);
    }
    else if(motor_value == MOTOR_RIGHT)
    {
        motor_stop(MOTOR_RIGHT);
        set_current_position(MOTOR_RIGHT, 0);
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(stop_obj, mpy_stop);


STATIC mp_obj_t mpy_get_motion_status(mp_obj_t self_in, mp_obj_t motor)
{
    int16_t motor_value = mp_obj_get_int(motor);
    step_motor_motion_type motion_type = STOP_MOVE; 
    if((motor_value == MOTOR_LEFT) || (motor_value == MOTOR_RIGHT))
    {
        motion_type = motor_get_motion_status(motor_value);
    }
    return mp_obj_new_int(motion_type);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(get_motion_status_obj, mpy_get_motion_status);

STATIC mp_obj_t mpy_get_motor_speed(mp_obj_t self_in, mp_obj_t motor)
{
    int16_t motor_value = mp_obj_get_int(motor);
    int32_t motor_speed = 0;
    if((motor_value == MOTOR_LEFT) || (motor_value == MOTOR_RIGHT))
    {
        motor_speed = motor_get_speed(motor_value);
    }
    return mp_obj_new_int(motor_speed);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(get_motor_speed_obj, mpy_get_motor_speed);

STATIC mp_obj_t mpy_set_max_speed(mp_obj_t self_in, mp_obj_t max_speed)
{
    uint32_t max_speed_value = mp_obj_get_int(max_speed);
    motor_set_move_speed_max(max_speed_value);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_max_speed_obj, mpy_set_max_speed);



STATIC const mp_rom_map_elem_t mpy_motion_locals_dict_table[] =
{
    { MP_OBJ_NEW_QSTR(MP_QSTR_forward),              (mp_obj_t)&forward_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_backward),             (mp_obj_t)&backward_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_turn_left),            (mp_obj_t)&turn_left_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_turn_right),           (mp_obj_t)&turn_right_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_move_position),        (mp_obj_t)&move_position_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_move_angle),           (mp_obj_t)&move_angle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_move_speed),           (mp_obj_t)&move_speed_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LEFT),                 MP_OBJ_NEW_SMALL_INT(MOTOR_LEFT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RIGHT),                MP_OBJ_NEW_SMALL_INT(MOTOR_RIGHT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ALL),                  MP_OBJ_NEW_SMALL_INT(MOTOR_MAX) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_motor_speed),          (mp_obj_t)&motor_speed_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_motor_pwm),            (mp_obj_t)&motor_pwm_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stop),                 (mp_obj_t)&stop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_motion_status),   (mp_obj_t)&get_motion_status_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_motor_speed),      (mp_obj_t)&get_motor_speed_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_max_speed),        (mp_obj_t)&set_max_speed_obj },
        
};

MP_DEFINE_CONST_DICT(mpy_motion_locals_dict, mpy_motion_locals_dict_table);

const mp_obj_type_t mpy_motion_type =
{
    { &mp_type_type },
    .name = MP_QSTR_motion,
    .print = mpy_motion_print,
    .make_new = mpy_motion_make_new,
    .locals_dict = (mp_obj_dict_t *)&mpy_motion_locals_dict,
};
#else /* MODULE_MOTION_ENABLE */
static const mp_map_elem_t mpy_motion_locals_dict_table[] =
{
};

static MP_DEFINE_CONST_DICT(mpy_motion_locals_dict, mpy_motion_locals_dict_table);

const mp_obj_type_t mpy_motion_type =
{
    { &mp_type_type },
    .name = MP_QSTR_motion,
    .locals_dict = (mp_obj_t)&mpy_motion_locals_dict,
};
#endif /* MODULE_MOTION_ENABLE */
