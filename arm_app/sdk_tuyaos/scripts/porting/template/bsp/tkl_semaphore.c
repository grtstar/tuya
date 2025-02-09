/**
 * @file tkl_semaphore.c
 * @brief this file was auto-generated by tuyaos v&v tools, developer can add implements between BEGIN and END
 *
 * @warning: changes between user 'BEGIN' and 'END' will be keeped when run tuyaos v&v tools
 *           changes in other place will be overwrited and lost
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 *
 */

// --- BEGIN: user defines and implements ---
#include "tkl_semaphore.h"
#include "tuya_error_code.h"
#include "tkl_adapter.h"

// --- END: user defines and implements ---

/**
 * @brief Create semaphore
 *
 * @param[out] handle: semaphore handle
 * @param[in] sem_cnt: semaphore init count
 * @param[in] sem_max: semaphore max count
 *
 * @note This API is used for creating semaphore.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tkl_semaphore_create_init(TKL_SEM_HANDLE *handle, UINT_T sem_cnt, UINT_T sem_max)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_SEM_HANDLE *handle, UINT_T sem_cnt, UINT_T sem_max);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_semaphore_create_init));

    if(fun) {
        ret = fun(handle, sem_cnt, sem_max);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Wait semaphore
 *
 * @param[in] handle: semaphore handle
 * @param[in] timeout: wait timeout, bet: ms. SEM_WAIT_FOREVER means wait until get semaphore
 *
 * @note This API is used for waiting semaphore.
 *
 * @return OPRT_OK on success.OPRT_OS_ADAPTER_SEM_WAIT_TIMEOUT means timeout. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tkl_semaphore_wait(CONST TKL_SEM_HANDLE handle, UINT_T timeout)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(CONST TKL_SEM_HANDLE handle, UINT_T timeout);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_semaphore_wait));

    if(fun) {
        ret = fun(handle, timeout);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Post semaphore
 *
 * @param[in] handle: semaphore handle
 *
 * @note This API is used for posting semaphore.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tkl_semaphore_post(CONST TKL_SEM_HANDLE handle)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(CONST TKL_SEM_HANDLE handle);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_semaphore_post));

    if(fun) {
        ret = fun(handle);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Release semaphore
 *
 * @param[in] handle: semaphore handle
 *
 * @note This API is used for releasing semaphore.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tkl_semaphore_release(CONST TKL_SEM_HANDLE handle)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(CONST TKL_SEM_HANDLE handle);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_semaphore_release));

    if(fun) {
        ret = fun(handle);
    }

    return ret;
    // --- END: user implements ---
}

