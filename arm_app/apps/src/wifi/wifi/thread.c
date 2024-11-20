#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include "wifi/os_net_utils.h"
#include "wifi/os_net_thread.h"

os_net_status_t os_net_thread_create(os_net_thread_t *thread_handle, const char *name,
                                     os_net_thread_func_t func, void *arg, os_net_thread_prio_t pri,
                                     uint32_t task_size)
{
    if (pthread_create(thread_handle, NULL, func, arg) != 0) {
        return OS_NET_STATUS_NOMEM;
    }
#ifdef _GNU_SOURCE
    if (pthread_setname_np(*thread_handle, name, strlen(name)) != 0) {
        OS_NET_ERROR("set name failed\n");
        return OS_NET_STATUS_NOMEM;
    }
#endif
    return OS_NET_STATUS_OK;
}

os_net_status_t os_net_thread_delete(os_net_thread_t *thread_handle)
{
    if (pthread_detach(*thread_handle) != 0) {
        return OS_NET_STATUS_FAILED;
    }
    if (pthread_cancel(*thread_handle) != 0) {
        return OS_NET_STATUS_FAILED;
    }
    return OS_NET_STATUS_OK;
}

os_net_status_t os_net_thread_get_pid(os_net_thread_pid_t *pid)
{
    *pid = pthread_self();
    return OS_NET_STATUS_OK;
}
