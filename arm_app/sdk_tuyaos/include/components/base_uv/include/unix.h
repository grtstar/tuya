#ifndef _UNIX_H_
#define _UNIX_H_

#include <pthread.h>

typedef pthread_t tuya_uv_thread_t;
typedef pthread_mutex_t tuya_uv_mutex_t;
typedef pthread_cond_t tuya_uv_cond_t;
typedef int tuya_uv_os_sock_t;
typedef struct tuya_uv_buf_t {
  char *base;
  size_t len;
} tuya_uv_buf_t;

#ifndef TUYA_UV_IO_PRIVATE_PLATFORM_FIELDS
#define TUYA_UV_IO_PRIVATE_PLATFORM_FIELDS /* empty */
#endif

struct tuya_uv__io_s;
struct tuya_uv_loop_s;
typedef void (*tuya_uv__io_cb)(struct tuya_uv_loop_s *loop,
                          struct tuya_uv__io_s *w,
                          unsigned int events);
typedef struct tuya_uv__io_s tuya_uv__io_t;

struct tuya_uv__io_s {
  tuya_uv__io_cb cb;
  void *pending_queue[2];
  void *watcher_queue[2];
  unsigned int pevents; /* Pending event mask i.e. mask at next tick. */
  unsigned int events;  /* Current event mask. */
  int fd;
  TUYA_UV_IO_PRIVATE_PLATFORM_FIELDS
};

#ifndef TUYA_UV_PLATFORM_LOOP_FIELDS
#define TUYA_UV_PLATFORM_LOOP_FIELDS /* empty */
#endif

#define TUYA_UV_LOOP_PRIVATE_FIELDS  \
  unsigned long flags;          \
  void *pending_queue[2];       \
  void *watcher_queue[2];       \
  tuya_uv__io_t **watchers;          \
  unsigned int nwatchers;       \
  unsigned int nfds;            \
  tuya_uv_handle_t *closing_handles; \
  struct                        \
  {                             \
    void *min;                  \
    unsigned int nelts;         \
  } timer_heap;                 \
  uint64_t timer_counter;       \
  uint64_t time;                \
  struct tuya_uv__work *works;       \
  int work_alloc_no;            \
  tuya_uv_mutex_t lock;

#define TUYA_UV_SHUTDOWN_PRIVATE_FIELDS /* empty */

#define TUYA_UV_TIMER_PRIVATE_FIELDS \
  tuya_uv_timer_cb timer_cb;         \
  void *heap_node[3];           \
  uint64_t timeout;             \
  uint64_t repeat;              \
  uint64_t start_id;            \
  uint16_t used;

#define TUYA_UV_UDP_PRIVATE_FIELDS \
  tuya_uv_udp_recv_cb recv_cb;     \
  tuya_uv_alloc_cb alloc_cb;       \
  tuya_uv__io_t io_watcher;        \
  void *write_queue[2];       \
  void *write_completed_queue[2];

#define TUYA_UV_UDP_SEND_PRIVATE_FIELDS \
  void *queue[2];                  \
  struct sockaddr_storage addr;    \
  unsigned int nbufs;              \
  tuya_uv_buf_t *bufs;                  \
  ssize_t status;                  \
  tuya_uv_udp_send_cb send_cb;          \
  tuya_uv_buf_t bufsml[4];

#ifndef TUYA_UV_STREAM_PRIVATE_PLATFORM_FIELDS
#define TUYA_UV_STREAM_PRIVATE_PLATFORM_FIELDS /* empty */
#endif

#define TUYA_UV_TCP_PRIVATE_FIELDS /* empty */

#define TUYA_UV_REQ_PRIVATE_FIELDS /* empty */

#define TUYA_UV_WRITE_PRIVATE_FIELDS \
  void *queue[2];               \
  unsigned int write_index;     \
  tuya_uv_buf_t *bufs;               \
  unsigned int nbufs;           \
  int error;                    \
  tuya_uv_buf_t bufsml[4];

#define TUYA_UV_CONNECT_PRIVATE_FIELDS \
  void *queue[2];

#define TUYA_UV_GETADDRINFO_PRIVATE_FIELDS \
  struct tuya_uv__work work_req;           \
  tuya_uv_getaddrinfo_cb cb;               \
  struct addrinfo *hints;             \
  char *hostname;                     \
  char *service;                      \
  struct addrinfo *addrinfo;          \
  int retcode;

#endif