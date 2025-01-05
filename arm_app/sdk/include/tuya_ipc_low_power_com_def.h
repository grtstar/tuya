/*
 * tuya_ipc_low_power_com_def.h
 *Copyright(C),2017-2022, TUYA company www.tuya.comm
 *
 *FILE description:
  *
 *  Created on: 2021年4月15日
 *      Author: kuiba
 */

#ifndef TY_IPC_LOW_POWER_SDK_SDK_SVC_LOWPOWER_INCLUDE_TUYA_IPC_LOW_POWER_COM_DEF_H_
#define TY_IPC_LOW_POWER_SDK_SDK_SVC_LOWPOWER_INCLUDE_TUYA_IPC_LOW_POWER_COM_DEF_H_
#include <stddef.h>
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef int OPERATE_RET;
#define OPRT_OK                             (0)
#define OPRT_COM_ERROR                      (-1)
#define OPRT_INVALID_PARM                   (-2)
#define OPRT_MALLOC_FAILED                  (-3)

typedef long long DLONG_T;
typedef DLONG_T *PDLONG_T;
typedef unsigned long long DDWORD_T;
typedef DDWORD_T *PDDWORD_T;
typedef float FLOAT_T;
typedef FLOAT_T *PFLOAT_T;
typedef signed int INT_T;
typedef int *PINT_T;
typedef void *PVOID_T;
typedef char CHAR_T;
typedef char *PCHAR_T;
typedef signed char SCHAR_T;
typedef unsigned char UCHAR_T;
typedef short SHORT_T;
typedef unsigned short USHORT_T;
typedef short *PSHORT_T;
typedef long LONG_T;
typedef unsigned long ULONG_T;
typedef long *PLONG_T;
typedef unsigned char BYTE_T;
typedef BYTE_T *PBYTE_T;
typedef unsigned short WORD_T;
typedef WORD_T *PWORD_T;
typedef unsigned int DWORD_T;
typedef DWORD_T *PDWORD_T;
typedef unsigned int UINT_T;
typedef unsigned int *PUINT_T;
typedef int BOOL_T;
typedef BOOL_T *PBOOL_T;
typedef long long int INT64_T;
typedef INT64_T *PINT64_T;
typedef unsigned long long int UINT64_T;
typedef UINT64_T *PUINT64_T;
typedef short INT16_T;
typedef INT16_T *PINT16_T;
typedef unsigned short UINT16_T;
typedef UINT16_T *PUINT16_T;
typedef char INT8_T;
typedef INT8_T *PINT8_T;
typedef unsigned char UINT8_T;
typedef UINT8_T *PUINT8_T;

typedef ULONG_T TIME_MS;
typedef ULONG_T TIME_S;
typedef unsigned int TIME_T;


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef VOID_T
#define VOID_T void
#endif


#ifndef CONST
#define CONST const
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef SIZEOF
#define SIZEOF sizeof
#endif

#ifndef INLINE
#define INLINE inline
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#ifndef bool_t
typedef int bool_t;
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

typedef size_t SIZE_T;

typedef unsigned int UNW_IP_ADDR_T;

#define PR_ERR(fmt, ...)    printf("Err:"fmt"\r\n", ##__VA_ARGS__)
#define PR_DEBUG(fmt, ...)  printf("Dbg:"fmt"\r\n", ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* TY_IPC_LOW_POWER_SDK_SDK_SVC_LOWPOWER_INCLUDE_TUYA_IPC_LOW_POWER_COM_DEF_H_ */
