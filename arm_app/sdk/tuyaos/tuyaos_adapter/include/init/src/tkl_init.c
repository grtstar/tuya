/**
 * @file tkl_init.c
 * @brief tuya kernel adapter layer init
 * @version 0.1
 * @date 2020-05-15
 * 
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 * 
 */

#include "tuya_iot_config.h"
#include "tkl_init.h"

/**
 *
 * @brief the description of tuya kernel ability, it will used by auto test tool
 *
 */
TKL_ABILITY_T g_tkl_ability = {0};

/**
 * @brief tuya kernel adapter layer init
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
OPERATE_RET tkl_init()
{
    return OPRT_OK;
}

/**
 * @brief get tuya kernel adapter layer version
 *
 * @return the version of the tuya kernel adapter layer
 *
 */
CHAR_T * tkl_get_version()
{
    return KERNEL_VER;
}

/**
 * @brief get tuya kernel adapter layer configure
 *
 * @return a json string descript the tuya kernel layer ablity
 *
 */
TKL_ABILITY_T * tkl_get_ability()
{

    // kernel support wired 
#if defined(ENABLE_WIRED) && (ENABLE_WIRED==1)
    g_tkl_ability.wired = TRUE;
#endif

    // kernel support wifi 
#if defined(ENABLE_WIFI) && (ENABLE_WIFI==1)
    g_tkl_ability.wifi = TRUE;
#endif

    // kernel support bluetooth
#if defined(ENABLE_BLUETOOTH) && (ENABLE_BLUETOOTH==1)
    g_tkl_ability.bt = TRUE;
#endif

    // kernel support nbiot
#if defined(ENABLE_NBIOT) && (ENABLE_NBIOT==1)
    g_tkl_ability.nbiot = TRUE;
#endif

    // kernel support cellular network, contains cat.1/cat.4/cat.m
#if defined(ENABLE_CELLULAR) && (ENABLE_CELLULAR==1)
    g_tkl_ability.cellular = TRUE;
    g_tkl_ability.cellular_mds = TRUE;
    g_tkl_ability.cellular_sms = TRUE;
    g_tkl_ability.cellular_vbat = TRUE;
#endif

    // kernel support flash
#if defined(ENABLE_FLASH) && (ENABLE_FLASH==1)
    g_tkl_ability.flash = TRUE;
#endif

    // kernel support uart
#if defined(ENABLE_UART) && (ENABLE_UART==1)
    g_tkl_ability.uart = TRUE;
#endif

    // kernel support watch dog
#if defined(ENABLE_WATCHDOG) && (ENABLE_WATCHDOG==1)
    g_tkl_ability.watchdog = TRUE;
#endif

    // kernel support rtc
#if defined(ENABLE_RTC) && (ENABLE_RTC==1)
    g_tkl_ability.rtc = TRUE;
#endif

    // kernel support adc
#if defined(ENABLE_ADC) && (ENABLE_ADC==1)
    g_tkl_ability.adc = TRUE;
#endif

    // kernel support pwm
#if defined(ENABLE_PWM) && (ENABLE_PWM==1)
    g_tkl_ability.pwm = TRUE;
#endif

    // kernel support i2c
#if defined(ENABLE_I2C) && (ENABLE_I2C==1)
    g_tkl_ability.i2c = TRUE;
#endif
    
    // kernel support spi
#if defined(ENABLE_SPI) && (ENABLE_SPI==1)
    g_tkl_ability.spi = TRUE;
#endif
    
#if defined(ENABLE_GPIO) && (ENABLE_GPIO==1)
    g_tkl_ability.gpio = TRUE;
#endif

    // kernel support pwm
#if defined(ENABLE_PWM) && (ENABLE_PWM==1)
    g_tkl_ability.pwm = TRUE;
#endif
    
    // kernel support hardware timer
#if defined(ENABLE_TIMER) && (ENABLE_TIMER==1)
    g_tkl_ability.timer = TRUE;
#endif
        
    // kernel support multi-media
#if defined(ENABLE_MEDIA) && (ENABLE_MEDIA==1)
    g_tkl_ability.media = TRUE;
#endif
        
    // kernel support lcd display
#if defined(ENABLE_DISPLAY) && (ENABLE_DISPLAY==1)
    g_tkl_ability.display = TRUE;
#endif

    // kernel support power management
#if defined(ENABLE_PM) && (ENABLE_PM==1)
    g_tkl_ability.pm = TRUE;
#endif

    return &g_tkl_ability;
}

