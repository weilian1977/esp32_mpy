#ifndef _MP_EXEC_CONTROL_H_
#define _MP_EXEC_CONTROL_H_

#include "mt_err.h"

#define SCRIPT_NAME_LENTH_MAX (32)

typedef enum
{
  SOFT_RESTART_TYPE_NONE,
  SOFT_RESTART_TYPE_FAST_RESTART,
  SOFT_RESTART_TYPE_WHOLE_RESTART,
}mp_soft_restart_type_t;

typedef enum
{
  /* the following three types are reserved */
  MP_EXEC_NONE = 0,
  MP_EXEC_REPEL_FRIENDLY = 1,
  MP_EXEC_REPEL_RAW = 2,
  MP_EXEC_TEST_SCRIPT = 3,

  /* change these types to adapt specific project */
  MP_EXEC_EMPTY_LOOP_SCRIPT,
  MP_EXEC_USER_SCRIPT,
  MP_EXEC_MAIN_LOOP_SCRIPT,
  MP_EXEC_ONLINE_SCRIPT,
  MP_EXEC_FACTORY_SCRIPT,

  MP_EXEC_MODE_MAX,
}mp_exec_mode_t;

typedef enum
{
  SCRIPT_TYPE_NONE,
  SCRIPT_TYPE_PY,
  SCRIPT_TYPE_MPY,
  SCRIPT_TYPE_INTERNEL_MPY,
}mp_script_type_t;

typedef struct
{
  mp_exec_mode_t mode;
  mp_script_type_t script_type;
  char script_name[SCRIPT_NAME_LENTH_MAX];
}mp_exec_item_t;

extern mt_err_t mp_exec_get_mode_t(mp_exec_mode_t *mode);
extern mt_err_t mp_exec_set_mode_t(mp_exec_mode_t mode);
extern bool mp_exec_is_mode_changed_t(void);

extern mt_err_t mp_exec_get_script_type_t(mp_script_type_t *type);
extern mt_err_t mp_exec_get_script_type_by_mode_t(mp_exec_mode_t mode, mp_script_type_t *type);

extern mt_err_t mp_exec_get_script_name_t(char *name);
extern mt_err_t mp_exec_get_script_name_by_mode_t(mp_exec_mode_t mode, char *name);

extern mt_err_t mp_exec_set_resatrt_flag_t(int sta);
extern mt_err_t mp_exec_get_resatrt_flag_t(int *sta);

extern mt_err_t mp_exec_set_temporary_running_script(char *module_name);
extern mt_err_t mp_exec_get_temporary_running_script(char *module_name);

#endif /* _MP_EXEC_CONTROL_H_ */