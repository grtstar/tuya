 /**
 * @file tal_i2c.h
 * @brief Common process - i2c
 * @version 0.1
 * @date 2021-08-26
 *
 * @copyright Copyright 2018-2021 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TAL_I2C_H__
#define __TAL_I2C_H__

#include "tuya_cloud_types.h"

/**
 * @brief i2c init
 * 
 * @param[in] i2c refer to tal_i2c_t
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_i2c_init(TUYA_I2C_NUM_E port_num, TUYA_IIC_BASE_CFG_T *cfg);

/**
 * @brief i2c control
 * 
 * @param[in] i2c refer to tal_i2c_t
 * @param[in] cmd control command
 * @param[in] arg control argument
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_i2c_control(TUYA_I2C_NUM_E port_num, UINT8_T cmd, VOID_T *arg);

/**
 * @brief i2c deinit
 * 
 * @param[in] i2c refer to tal_i2c_t
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_i2c_deinit(TUYA_I2C_NUM_E port_num);

/**
 * @brief i2c master send
 * 
 * @param[in] i2c refer to tal_i2c_t
 * @param[in] addr device address
 * @param[in] buf send buf
 * @param[in] count send length
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_i2c_master_send(TUYA_I2C_NUM_E port_num, UINT16_T addr, VOID_T *buf, UINT16_T count);

/**
 * @brief i2c master recv
 * 
 * @param[in] i2c refer to tal_i2c_t
 * @param[in] addr device address
 * @param[in] buf recv buf
 * @param[in] count recv length
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_i2c_master_receive(TUYA_I2C_NUM_E port_num, UINT16_T addr, VOID_T *buf, UINT16_T count);

/**
 * @brief i2c irq init
 * NOTE: call this API will not enable interrupt
 * 
 * @param[in] port: i2c port, id index starts at 0
 * @param[in] cfg:  i2c irq config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_i2c_irq_init(TUYA_I2C_NUM_E port_num, CONST TUYA_I2C_IRQ_CB *cb);

/**
 * @brief i2c irq enable
 * 
 * @param[in] port: i2c port id, id index starts at 0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_i2c_irq_enable(TUYA_I2C_NUM_E port_num);

/**
 * @brief i2c irq disable
 * 
 * @param[in] port: i2c port id, id index starts at 0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_i2c_irq_disable(TUYA_I2C_NUM_E port_num);

/**
 * @brief i2c slave
 * 
 * @param[in] port: i2c port
 * @param[in] dev_addr: slave device addr
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_i2c_set_slave_addr(TUYA_I2C_NUM_E port_num, UINT16_T dev_addr);

/**
 * @brief i2c slave send
 * 
 * @param[in] port: i2c port
 * @param[in] data: i2c buf to send
 * @param[in] size: Number of data items to send
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */

OPERATE_RET tal_i2c_slave_send(TUYA_I2C_NUM_E port_num, CONST VOID *data, UINT32_T size);

/**
 * @brief IIC slave receive, Start receiving data as IIC Slave.
 * 
 * @param[in] port: i2c port
 * @param[in] data: Pointer to buffer for data to receive from IIC Master
 * @param[in] size: Number of data items to receive
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */

OPERATE_RET tal_i2c_slave_receive(TUYA_I2C_NUM_E port_num, VOID *data, UINT32_T size);

/**
 * @brief IIC get status.
 * 
 * @param[in] port: i2c port
 * @return TUYA_IIC_STATUS_T,please refer to tuya_cloud_types.h
 */
OPERATE_RET tal_i2c_get_status(TUYA_I2C_NUM_E port_num, TUYA_IIC_STATUS_T *status);

/**
 * @brief i2c's reset
 * 
 * @param[in] port: i2c port number
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET  tal_i2c_reset(TUYA_I2C_NUM_E port_num);


#endif
