#ifndef __OS_NET_SYNC_NOTIFY_H__
#define __OS_NET_SYNC_NOTIFY_H__
#include <stdint.h>
#include <stdbool.h>
#include "wifi/os_net_utils.h"
#include "wifi/os_net_sem.h"

typedef struct {
    bool ready;
    os_net_sem_t sem;
    void *result;
} snfy_handle_t;

snfy_handle_t *snfy_new(void);

os_net_status_t snfy_ready(snfy_handle_t *handle, void *value);

void *snfy_await(snfy_handle_t *handle);

os_net_status_t snfy_free(snfy_handle_t *handle);
#endif
