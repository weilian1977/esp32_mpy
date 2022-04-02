#include <stdint.h>
#include <stdio.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpstate.h"
#include "py/nlr.h"
#include "drv_i2c.h"
//#include "drv_qmc6310.h"
#include "drv_qmi8658.h"
#include "drv_motion_sensor.h"
#include "string.h"

//#include "drv_button.h"


/*
STATIC mp_obj_t get_field_strength(){}
*/
//float	mag_data[3];
float	angle_data[3];
/*
STATIC mp_obj_t mpy_get_mag_x()
{
    qmc6310_read_mag_xyz(mag_data);
    return mp_obj_new_float(mag_data[0]);
}

STATIC mp_obj_t mpy_get_mag_y()
{
    qmc6310_read_mag_xyz(mag_data);
    return mp_obj_new_float(mag_data[1]);
}

STATIC mp_obj_t mpy_get_mag_z()
{
    qmc6310_read_mag_xyz(mag_data);
    return mp_obj_new_float(mag_data[2]);
}

STATIC mp_obj_t mpy_compass_calibrate()
{
    magne_calibration();
    return mp_const_none;
}

STATIC mp_obj_t mpy_compass_heading()
{
    qmc6310_read_mag_xyz(mag_data);
    return mp_obj_new_float(compass_angle);
}
*/
STATIC mp_obj_t mpy_get_roll()
{
    IMU_GetYawPitchRoll(angle_data);
    return mp_obj_new_float(angle_data[2]);
}

STATIC mp_obj_t mpy_get_pitch()
{
    IMU_GetYawPitchRoll(angle_data);
    return mp_obj_new_float(angle_data[1]);
}

STATIC mp_obj_t mpy_get_yaw()
{
    IMU_GetYawPitchRoll(angle_data);
    return mp_obj_new_float(angle_data[0]);
}

STATIC mp_obj_t mpy_sensor_calibrate()
{
    qmi8658_calibration();
    return mp_const_none;
}

STATIC mp_obj_t mpy_reset_rotation()
{
    reset_pos();
    return mp_const_none;
}


STATIC mp_obj_t mpy_get_rotation(mp_obj_t axis)
{
    const char * axis_xyz = mp_obj_str_get_str(axis);

    IMU_GetYawPitchRoll(angle_data);
    if(strcmp(axis_xyz,"x") ==0)
    {
        return mp_obj_new_float(angle_data[1]);
    }
    else if(strcmp(axis_xyz,"y") ==0)
    {
        return mp_obj_new_float(angle_data[2]);
    }
    else if(strcmp(axis_xyz,"z") ==0)
    {
        return mp_obj_new_float(angle_data[0]);
    }
    else
    {
        printf("not set success xyz\n");
    }
    return mp_const_none;
}

STATIC mp_obj_t mpy_qmi8658_init(void)
{
    Qmi8658_init();
    return mp_const_none;
}
/*
STATIC mp_obj_t mpy_qmc6310_init(void)
{
    qmc6310_init();
    return mp_const_none;
}
*/
STATIC mp_obj_t mpy_get_acceleration(mp_obj_t axis)
{
    const char * axis_xyz = mp_obj_str_get_str(axis);

    get_acc_gyro_angle();
    if(strcmp(axis_xyz,"x") ==0)
    {
        return mp_obj_new_float(acc_x);
    }

    else if(strcmp(axis_xyz,"y") ==0)
    {
        return mp_obj_new_float(acc_y);
    }

    else if(strcmp(axis_xyz,"z") ==0)
    {
        return mp_obj_new_float(acc_z);
    }
    else 
    {
        printf("not set success xyz\n");
    }
    return mp_const_none;
    /*
    if(strcmp(axis_xyz,"xyz") ==0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(acc_x),
                                             mp_obj_new_float(acc_y),
                                             mp_obj_new_float(acc_z),
                                            });
    }
    else if (strcmp(axis_xyz,"xzy") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(acc_x),
                                             mp_obj_new_float(acc_z),
                                             mp_obj_new_float(acc_y),
                                            });
    }
    else if (strcmp(axis_xyz,"yxz") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(acc_y),
                                             mp_obj_new_float(acc_x),
                                             mp_obj_new_float(acc_z),
                                            });
    }
    else if (strcmp(axis_xyz,"yzx") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(acc_y),
                                             mp_obj_new_float(acc_z),
                                             mp_obj_new_float(acc_x),
                                            });
    }
    else if (strcmp(axis_xyz,"zxy") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(acc_z),
                                             mp_obj_new_float(acc_x),
                                             mp_obj_new_float(acc_y),
                                            });
    }
    else if (strcmp(axis_xyz,"zyx") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(acc_z),
                                             mp_obj_new_float(acc_y),
                                             mp_obj_new_float(acc_x),
                                            });
    }
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(acc_x),
                                             mp_obj_new_float(acc_y),
                                             mp_obj_new_float(acc_z),
                                            });
    */
}

STATIC mp_obj_t mpy_get_gyroscope(mp_obj_t axis)
{
    const char * axis_xyz = mp_obj_str_get_str(axis);
    get_acc_gyro_angle();

    if(strcmp(axis_xyz,"x") ==0)
    {
        return mp_obj_new_float(gyro_x);
    }

    else if(strcmp(axis_xyz,"y") ==0)
    {
        return mp_obj_new_float(gyro_y);
    }

    else if(strcmp(axis_xyz,"z") ==0)
    {
        return mp_obj_new_float(gyro_z);
    }
    else 
    {
        printf("not set success xyz\n");
    }
    return mp_const_none;
    /*
    if(strcmp(axis_xyz,"xyz") ==0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(gyro_x),
                                             mp_obj_new_float(gyro_y),
                                             mp_obj_new_float(gyro_z),
                                            });
    }
    else if (strcmp(axis_xyz,"xzy") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(gyro_x),
                                             mp_obj_new_float(gyro_z),
                                             mp_obj_new_float(gyro_y),
                                            });
    }
    else if (strcmp(axis_xyz,"yxz") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(gyro_y),
                                             mp_obj_new_float(gyro_x),
                                             mp_obj_new_float(gyro_z),
                                            });
    }
    else if (strcmp(axis_xyz,"yzx") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(gyro_y),
                                             mp_obj_new_float(gyro_z),
                                             mp_obj_new_float(gyro_x),
                                            });
    }
    else if (strcmp(axis_xyz,"zxy") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(gyro_z),
                                             mp_obj_new_float(gyro_x),
                                             mp_obj_new_float(gyro_y),
                                            });
    }
    else if (strcmp(axis_xyz,"zyx") == 0)
    {
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(gyro_z),
                                             mp_obj_new_float(gyro_y),
                                             mp_obj_new_float(gyro_x),
                                            });
    }
    return mp_obj_new_tuple(3,(mp_obj_t []) {mp_obj_new_float(gyro_x),
                                             mp_obj_new_float(gyro_y),
                                             mp_obj_new_float(gyro_z),
                                            });
    */

}
/*
STATIC mp_obj_t mpy_get_mag_data(void)
{

    qmc6310_read_mag_xyz(mag_data);

    return mp_obj_new_tuple(3, (mp_obj_t []) {mp_obj_new_float(mag_data[0]),
                                              mp_obj_new_float(mag_data[1]), 
                                              mp_obj_new_float(mag_data[2])});
}
*/
/*
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_get_mag_x_obj, mpy_get_mag_x);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_get_mag_y_obj, mpy_get_mag_y);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_get_mag_z_obj, mpy_get_mag_z);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_compass_calibrate_obj, mpy_compass_calibrate);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_compass_heading_obj, mpy_compass_heading);
*/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_get_roll_obj, mpy_get_roll);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_get_pitch_obj, mpy_get_pitch);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_get_yaw_obj, mpy_get_yaw);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_get_rotation_obj, mpy_get_rotation);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_reset_rotation_obj, mpy_reset_rotation);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_qmi8658_init_obj, mpy_qmi8658_init);
//STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_qmc6310_init_obj, mpy_qmc6310_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_get_acceleration_obj, mpy_get_acceleration);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_get_gyroscope_obj, mpy_get_gyroscope);
//STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_get_mag_data_obj, mpy_get_mag_data);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_sensor_calibrate_obj, mpy_sensor_calibrate);


STATIC const mp_map_elem_t motion_sensor_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_qmi8658_init),                (mp_obj_t)&mpy_qmi8658_init_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_qmc6310_init),                (mp_obj_t)&mpy_qmc6310_init_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_get_mag_data),                (mp_obj_t)&mpy_get_mag_data_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_acceleration),            (mp_obj_t)&mpy_get_acceleration_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_gyroscope),               (mp_obj_t)&mpy_get_gyroscope_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_roll),                    (mp_obj_t)&mpy_get_roll_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_pitch),                   (mp_obj_t)&mpy_get_pitch_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_yaw),                     (mp_obj_t)&mpy_get_yaw_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_rotation),                (mp_obj_t)&mpy_get_rotation_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset_rotation),              (mp_obj_t)&mpy_reset_rotation_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_get_mag_x),                   (mp_obj_t)&mpy_get_mag_x_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_get_mag_y),                   (mp_obj_t)&mpy_get_mag_y_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_get_mag_z),                   (mp_obj_t)&mpy_get_mag_z_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_compass_calibrate),           (mp_obj_t)&mpy_compass_calibrate_obj },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_compass_heading),             (mp_obj_t)&mpy_compass_heading_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sensor_calibrate),            (mp_obj_t)&mpy_sensor_calibrate_obj },
    

};

STATIC MP_DEFINE_CONST_DICT(motion_sensor_module_globals, motion_sensor_module_globals_table);

const mp_obj_module_t motion_sensor_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&motion_sensor_module_globals,
};


