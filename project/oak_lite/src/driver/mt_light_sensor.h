#ifndef _MT_LIGHT_SENSOR_H_
#define _MT_LIGHT_SENSOR_H_

#include "mt_err.h"

mt_err_t mt_light_sensor_init_t(void);
mt_err_t mt_light_sensor_update_t(void);
mt_err_t mt_light_sensor_get_value_t(int *value);

#endif /* _MT_LIGHT_SENSOR_H_ */
