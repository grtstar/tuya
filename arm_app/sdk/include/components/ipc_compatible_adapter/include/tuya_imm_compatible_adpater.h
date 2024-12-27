/*
 * tuya_imm_compatible_adpater.h
 *Copyright(C),2017-2022, TUYA company www.tuya.com
 *
 *FILE description:
  *
 *  Created on: 2022年5月6日
 *      Author: kuiba
 */

#ifndef COMPONENTS_IPC_COMPATIBLE_ADAPTER_INCLUDE_TUYA_IMM_COMPATIBLE_ADPATER_H_
#define COMPONENTS_IPC_COMPATIBLE_ADAPTER_INCLUDE_TUYA_IMM_COMPATIBLE_ADPATER_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "tuya_cloud_types.h"
#include "tuya_error_code.h"


#define DEV_CHAN_MAX_NUM 1

OPERATE_RET tuya_imm_dev_chan_get_by_devId(IN CONST CHAR_T* devId, OUT INT_T* pChn);
OPERATE_RET tuya_imm_dev_devId_get_by_chan(IN CONST INT_T chn, OUT CHAR_T* devId, IN CONST INT_T len);

#ifdef __cplusplus
}
#endif
#endif /* COMPONENTS_IPC_COMPATIBLE_ADAPTER_INCLUDE_TUYA_IMM_COMPATIBLE_ADPATER_H_ */
