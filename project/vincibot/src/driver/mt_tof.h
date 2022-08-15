#ifndef _MT_TOF_H_
#define _MT_TOF_H_

#include "mt_err.h"

mt_err_t mt_tof_init_t(void);
mt_err_t mt_tof_update_t(void);
mt_err_t mt_tof_get_distance_t(float *distance);
mt_err_t mt_tof_offset_calibration();

#endif /* _MT_TOF_H_ */
