#ifndef _MT_ERR_H_
#define _MT_ERR_H_

/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define MT_OK                      (0)
#define MT_FAIL                    (-1)
 
#define MT_ERR_NO_MEM              (0x101)
#define MT_ERR_INVALID_ARG         (0x102)
#define MT_ERR_INVALID_STATE       (0x103)
#define MT_ERR_INVALID_SIZE        (0x104)
#define MT_ERR_NOT_FOUND           (0x105)
#define MT_ERR_NOT_SUPPORTED       (0x106)
#define MT_ERR_TIMEOUT             (0x107)
#define MT_ERR_INVALID_RESPONSE    (0x108)
#define MT_ERR_BUFFER_OVER         (0x109)
#define MT_ERR_INVALID_VERSION     (0x10A)
#define MT_ERR_INVALID_MAC         (0x10B)

#define FUNCTION_RET_CHECK(a, str, ret_val) 
    //if((a)) 
    //{ 
      //MT_LOGE(TAG, "%s(%d):err_id(%d), %s", __FUNCTION__, __LINE__, a, str); 
      //return (ret_val); 
    //}

#define FUNCTION_RET_CHECK_WITHOUT_RETURN(a, str, ret_val) 
    //if((a)) 
    //{ 
      //MT_LOGE(TAG, "%s(%d):err_id(%d), %s", __FUNCTION__, __LINE__, a, str); 
    //}

/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/
typedef int32_t mt_err_t;

#endif /* _MT_ERR_H_ */