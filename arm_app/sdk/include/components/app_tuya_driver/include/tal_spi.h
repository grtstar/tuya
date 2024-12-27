 /**
 * @file tal_spi.h
 * @brief Common process - driver spi
 * @version 0.1
 * @date 2019-08-20
 *
 * @copyright Copyright 2021-2025 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TAL_SPI_H__
#define __TAL_SPI_H__

#include "tuya_cloud_types.h"

typedef struct {
    VOID            *send_buf;
    VOID            *recv_buf;
    UINT32_T         length;
} TAL_SPI_MSG_T;

/**
 * @brief spi init
 * 
 * @param[in] spi_bus refer to tal_spi_bus_t
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_spi_init(UINT_T port_num, TUYA_SPI_BASE_CFG_T *cfg);


/**
 * @brief spi transfer
 * 
 * @param[in] spi refer to tal_spi_t
 * @param[in] send_buf used to send
 * @param[in] recv_buf used to recv
 * @param[in] length message length
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_spi_xfer(UINT_T port_num, VOID_T *send_buf, VOID_T *recv_buf, UINT_T length);

/**
 * @brief spi xfer msg
 * 
 * @param[in] spi refer to tal_spi_t
 * @param[in] msg refer to tal_spi_msg_t
 * @param[in] num message number
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_spi_xfer_msg(UINT_T port_num, TAL_SPI_MSG_T *msg, UINT8_T num);


/**
 * @brief spi control
 * 
 * @param[in] spi refer to tal_spi_t
 * @param[in] cmd control command
 * @param[in] arg command argument
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_spi_control(UINT_T port_num, UINT8_T cmd, VOID_T *arg);

/**
 * @brief spi deinit
 * 
 * @param[in] spi refer to tal_spi_t
 *
 * @return OPRT_OK on success. Others on error, please refer to tal_error_code.h
 */
OPERATE_RET tal_spi_deinit(UINT_T port_num);


/**
 * Spi send
 *
 * @param[in]  port      the spi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 *
 * @return  OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_spi_send(TUYA_SPI_NUM_E port_num, VOID_T *data, UINT16_T size);

/**
 * spi_recv
 *
 * @param[in]   port      the spi device
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 *
 * @return  OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_spi_recv(TUYA_SPI_NUM_E port_num, VOID_T *data, UINT16_T size);

/**
 * @brief adort spi transfer,or spi send, or spi recv
 * 
 * @param[in] port: spi port
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

OPERATE_RET tal_spi_abort_transfer(TUYA_SPI_NUM_E port_num);

/**
 * @brief spi irq init
 * NOTE: call this API will not enable interrupt
 * 
 * @param[in] port: spi port, id index starts at 0
 * @param[in] cfg:  spi irq config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_spi_irq_init(TUYA_SPI_NUM_E port_num, CONST TUYA_SPI_IRQ_CB *cb);

/**
 * @brief spi irq enable
 * 
 * @param[in] port: spi port id, id index starts at 0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_spi_irq_enable(TUYA_SPI_NUM_E port_num);

/**
 * @brief spi irq disable
 * 
 * @param[in] port: spi port id, id index starts at 0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_spi_irq_disable(TUYA_SPI_NUM_E port_num);

/**
 * @brief spi get status.
 * 
 * @param[in] port: spi port
 * @return TUYA_IIC_STATUS_T,please refer to tuya_cloud_types.h
 */
OPERATE_RET tal_spi_get_status(TUYA_SPI_NUM_E port_num, TUYA_SPI_STATUS_T *status);



#define tal_spi_send_then_send(spi, send1, len1, send2, len2)              \
    do {                                                                    \
        TAL_SPI_MSG_T    msg[2];                                           \
        msg[0].send_buf = send1;                                            \
        msg[0].recv_buf = NULL;                                             \
        msg[0].length   = len1;                                             \
        msg[1].send_buf = send2;                                            \
        msg[1].recv_buf = NULL;                                             \
        msg[1].length   = len2;                                             \
        tal_spi_xfer_msg(spi, msg, 2);                                     \
    } while (0)

#define tal_spi_send_then_recv(spi, send, len1, recv, len2)                \
    do {                                                                    \
        TAL_SPI_MSG_T    msg[2];                                           \
        msg[0].send_buf = send;                                             \
        msg[0].recv_buf = NULL;                                             \
        msg[0].length   = len1;                                             \
        msg[1].send_buf = NULL;                                             \
        msg[1].recv_buf = recv;                                             \
        msg[1].length   = len2;                                             \
        tal_spi_xfer_msg(spi, msg, 2);                                     \
    } while (0)

#endif

