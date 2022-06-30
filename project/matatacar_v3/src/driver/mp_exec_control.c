#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "mt_err.h"
#include "mt_log.h"
#include "mp_exec_control.h"

/*****************************************************************
 DEFINE MACROS
******************************************************************/
#define TAG               ("MP_EXEC_CONTROL")

/*****************************************************************
 DEFINE TYPES & CONSTANS
******************************************************************/

/*****************************************************************
DEFINE PRIVATE DATAS
******************************************************************/
static const mp_exec_item_t s_mp_mode_type[MP_EXEC_MODE_MAX] = 
{
  {MP_EXEC_NONE, SCRIPT_TYPE_NONE, {""}},
  {MP_EXEC_REPEL_FRIENDLY, SCRIPT_TYPE_NONE, {""}},
  {MP_EXEC_REPEL_RAW, SCRIPT_TYPE_NONE, {""}},
  {MP_EXEC_TEST_SCRIPT, SCRIPT_TYPE_INTERNEL_MPY, {"test.py"}},
  {MP_EXEC_EMPTY_LOOP_SCRIPT, SCRIPT_TYPE_INTERNEL_MPY, {"empty_loop.py"}}, 
  {MP_EXEC_USER_SCRIPT, SCRIPT_TYPE_PY, {"main.py"}},
  {MP_EXEC_ONLINE_SCRIPT, SCRIPT_TYPE_INTERNEL_MPY, {"empty_loop.py"}},
  {MP_EXEC_FACTORY_SCRIPT, SCRIPT_TYPE_INTERNEL_MPY, {"factory_script.py"}},
};

struct mp_exec_control_t
{
  mp_exec_mode_t current_mode;
  bool is_mode_changed;
  int restart_flag;
  char temporary_module[32];
}mp_exec_control;

/*****************************************************************
DECLARE PRIVATE FUNCTIONS
******************************************************************/


/*****************************************************************
 DEFINE PUBLIC FUNCTIONS
******************************************************************/
mt_err_t mp_exec_get_mode_t(mp_exec_mode_t *mode)
{
  (*mode) = mp_exec_control.current_mode;
  return MT_OK;
}

mt_err_t mp_exec_set_mode_t(mp_exec_mode_t mode)
{
  if((mode < MP_EXEC_NONE) || (mode >= MP_EXEC_MODE_MAX))
  {
    MT_LOGE(TAG, "set mode error, error mode is:%d, current mode is:%d", mode, mp_exec_control.current_mode);
    return MT_ERR_INVALID_ARG;
  }
  if(mp_exec_control.current_mode != mode)
  {
    mp_exec_control.is_mode_changed = true;
    mp_exec_control.current_mode = mode;
  }
  else
  {
    mp_exec_control.is_mode_changed = false;
  }
  return MT_OK;
}

bool mp_exec_is_mode_changed_t(void)
{
  if(mp_exec_control.is_mode_changed)
  {
    mp_exec_control.is_mode_changed = false;
    return true;
  }
  else
  {
    return false;
  }
}

mt_err_t mp_exec_get_script_type_t(mp_script_type_t *type)
{
  (*type) = s_mp_mode_type[mp_exec_control.current_mode].script_type;
  return MT_OK;
}

mt_err_t mp_exec_get_script_type_by_mode_t(mp_exec_mode_t mode, mp_script_type_t *type)
{
  if((mode < MP_EXEC_NONE) || (mode >= MP_EXEC_MODE_MAX))
  {
    MT_LOGE(TAG, "get script type error, error mode is:%d, current mode is:%d", mode, mp_exec_control.current_mode);
    return MT_ERR_INVALID_ARG;
  }
   
  (*type) = s_mp_mode_type[mode].script_type;
  return MT_OK;
}

mt_err_t mp_exec_get_script_name_t(char *name)
{
  strcpy(name, s_mp_mode_type[mp_exec_control.current_mode].script_name);
  return MT_OK;
}

mt_err_t mp_exec_get_script_name_by_mode_t(mp_exec_mode_t mode, char *name)
{
  if((mode < MP_EXEC_NONE) || (mode >= MP_EXEC_MODE_MAX))
  {
    MT_LOGE(TAG, "get name error, error mode is:%d, current mode is:%d", mode, mp_exec_control.current_mode);
    return MT_ERR_INVALID_ARG;
  }
   
  strcpy(name, s_mp_mode_type[mode].script_name);
  return MT_OK;
}

mt_err_t mp_exec_set_resatrt_flag_t(int sta)
{
  mp_exec_control.restart_flag = sta;
  return MT_OK;
}

mt_err_t mp_exec_get_resatrt_flag_t(int *sta)
{
  *sta = mp_exec_control.restart_flag;
  return MT_OK;
}

mt_err_t mp_exec_set_temporary_running_script(char *module_name)
{
  strcpy(mp_exec_control.temporary_module, module_name);
  return MT_OK;
}

mt_err_t mp_exec_get_temporary_running_script(char *module_name)
{
  strcpy(module_name, mp_exec_control.temporary_module);
  return MT_OK;
}

/*****************************************************************
 DEFINE PRIVATE FUNCTIONS
******************************************************************/