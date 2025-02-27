/**
 * @file tkl_display.c
 * @brief this file was auto-generated by tuyaos v&v tools, developer can add implements between BEGIN and END
 *
 * @warning: changes between user 'BEGIN' and 'END' will be keeped when run tuyaos v&v tools
 *           changes in other place will be overwrited and lost
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 *
 */

// --- BEGIN: user defines and implements ---
#include "tkl_display.h"
#include "tuya_error_code.h"
#include "tkl_adapter.h"

// --- END: user defines and implements ---

/**
 * @brief Init and config display device
 *
 * @param display_device display device
 * @param cfg display configuration
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_init(TKL_DISP_DEVICE_S *display_device, TKL_DISP_EVENT_HANDLER_S *event_handler)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_EVENT_HANDLER_S *event_handler);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_init));

    if(fun) {
        ret = fun(display_device, event_handler);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Release display device
 *
 * @param display_device display device
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_deinit(TKL_DISP_DEVICE_S *display_device)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_deinit));

    if(fun) {
        ret = fun(display_device);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Set display info
 *
 * @param display_device display device
 * @param info display device info
 * @return OPERATE_RET
 */

OPERATE_RET tkl_disp_set_info(TKL_DISP_DEVICE_S *display_device, TKL_DISP_INFO_S *info)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_INFO_S *info);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_set_info));

    if(fun) {
        ret = fun(display_device, info);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Get display info
 *
 * @param display_device display device
 * @param info display device info
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_get_info(TKL_DISP_DEVICE_S *display_device, TKL_DISP_INFO_S *info)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_INFO_S *info);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_get_info));

    if(fun) {
        ret = fun(display_device, info);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief
 *
 * @param display_device display device
 * @param buf framebuffer
 * @param rect destination area
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_blit(TKL_DISP_DEVICE_S *display_device, TKL_DISP_FRAMEBUFFER_S *buf, TKL_DISP_RECT_S *rect)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_FRAMEBUFFER_S *buf, TKL_DISP_RECT_S *rect);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_blit));

    if(fun) {
        ret = fun(display_device, buf, rect);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Fill destination area with color
 *
 * @param display_device display device
 * @param rect destination area to fill
 * @param color color to fill
 * @return OPERATE_RET
 */

OPERATE_RET tkl_disp_fill(TKL_DISP_DEVICE_S *display_device, TKL_DISP_RECT_S *rect, TKL_DISP_COLOR_U color)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_RECT_S *rect, TKL_DISP_COLOR_U color);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_fill));

    if(fun) {
        ret = fun(display_device, rect, color);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Flush buffers to display device
 *
 * @param display_device display device
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_flush(TKL_DISP_DEVICE_S *display_device)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_flush));

    if(fun) {
        ret = fun(display_device);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Wait for vsync signal
 *
 * @param display_device display device
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_wait_vsync(TKL_DISP_DEVICE_S *display_device)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_wait_vsync));

    if(fun) {
        ret = fun(display_device);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Set display brightness(Backlight or HSB)
 *
 * @param display_device display device
 * @param brightness brightness
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_set_brightness(TKL_DISP_DEVICE_S *display_device, INT_T brightness)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, INT_T brightness);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_set_brightness));

    if(fun) {
        ret = fun(display_device, brightness);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Get display brightness(Backlight or HSB)
 *
 * @param display_device display device
 * @param brightness brightness
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_get_brightness(TKL_DISP_DEVICE_S *display_device, INT_T *brightness)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, INT_T *brightness);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_get_brightness));

    if(fun) {
        ret = fun(display_device, brightness);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Sets the display screen's power state
 *
 * @param display_device display device
 * @param power_mode power state
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_set_power_mode(TKL_DISP_DEVICE_S *display_device, TKL_DISP_POWER_MODE_E power_mode)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_POWER_MODE_E power_mode);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_set_power_mode));

    if(fun) {
        ret = fun(display_device, power_mode);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Gets the display screen's power state
 *
 * @param display_device display device
 * @param power_mode power state
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_get_power_mode(TKL_DISP_DEVICE_S *display_device, TKL_DISP_POWER_MODE_E *power_mode)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_POWER_MODE_E *power_mode);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_get_power_mode));

    if(fun) {
        ret = fun(display_device, power_mode);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Alloc mapped framebuffer or layer
 *
 * @param display_device display device
 * @return VOID_T* Pointer to mapped memory
 */

TKL_DISP_FRAMEBUFFER_S* tkl_disp_alloc_framebuffer(TKL_DISP_DEVICE_S *display_device)
{
    // --- BEGIN: user implements ---
    typedef TKL_DISP_FRAMEBUFFER_S* (*TYPE_T)(TKL_DISP_DEVICE_S *display_device);

    TKL_DISP_FRAMEBUFFER_S* ret = 0;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_alloc_framebuffer));

    if(fun) {
        ret = fun(display_device);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Free mapped framebuffer or layer
 *
 * @param display_device display device
 * @param buf Pointer to mapped memory
 * @return VOID_T
 */

VOID_T tkl_disp_free_framebuffer(TKL_DISP_DEVICE_S *display_device, TKL_DISP_FRAMEBUFFER_S *buf)
{
    // --- BEGIN: user implements ---
    typedef VOID_T (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_FRAMEBUFFER_S *buf);

    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_free_framebuffer));

    if(fun) {
         fun(display_device, buf);
    }

    return;
    // --- END: user implements ---
}

/**
 * @brief Get capabilities supported by display(For external display device. e.g. HDMI/VGA)
 *
 * @param display_device display device
 * @param cfg configurations
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_get_capabilities(TKL_DISP_DEVICE_S *display_device, TKL_DISP_INFO_S **cfg)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_INFO_S **cfg);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_get_capabilities));

    if(fun) {
        ret = fun(display_device, cfg);
    }

    return ret;
    // --- END: user implements ---
}

/**
 * @brief Free capabilities get by tkl_disp_get_capabilities()
 *
 * @param display_device display device
 * @param cfg configurations
 * @return OPERATE_RET 0 on success. A negative error code on error.
 */

OPERATE_RET tkl_disp_free_capabilities(TKL_DISP_DEVICE_S *display_device, TKL_DISP_INFO_S *cfg)
{
    // --- BEGIN: user implements ---
    typedef OPERATE_RET (*TYPE_T)(TKL_DISP_DEVICE_S *display_device, TKL_DISP_INFO_S *cfg);

    OPERATE_RET ret = OPRT_NOT_SUPPORTED;
    TYPE_T fun;

    fun = (TYPE_T)tkl_adapt_func_get(FUNC_NAME(tkl_disp_free_capabilities));

    if(fun) {
        ret = fun(display_device, cfg);
    }

    return ret;
    // --- END: user implements ---
}

