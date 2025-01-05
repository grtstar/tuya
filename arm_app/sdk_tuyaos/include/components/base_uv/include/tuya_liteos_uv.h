#ifndef __TUYA_LITEOS_TUYA_UV_H__
#define __TUYA_LITEOS_TUYA_UV_H__

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
//#include <linux/in.h>
//#include <linux/in6.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include "unix.h"
#include "tuya_uv_errno.h"
#ifdef __cplusplus
extern "C"
{
#endif

/* Expand this list if necessary. */
#define TUYA_UV_ERRNO_MAP(XX)                                                      \
  XX(E2BIG, "argument list too long")                                         \
  XX(EACCES, "permission denied")                                             \
  XX(EADDRINUSE, "address already in use")                                    \
  XX(EADDRNOTAVAIL, "address not available")                                  \
  XX(EAFNOSUPPORT, "address family not supported")                            \
  XX(EAGAIN, "resource temporarily unavailable")                              \
  XX(EAI_ADDRFAMILY, "address family not supported")                          \
  XX(EAI_AGAIN, "temporary failure")                                          \
  XX(EAI_BADFLAGS, "bad ai_flags value")                                      \
  XX(EAI_BADHINTS, "invalid value for hints")                                 \
  XX(EAI_CANCELED, "request canceled")                                        \
  XX(EAI_FAIL, "permanent failure")                                           \
  XX(EAI_FAMILY, "ai_family not supported")                                   \
  XX(EAI_MEMORY, "out of memory")                                             \
  XX(EAI_NODATA, "no address")                                                \
  XX(EAI_NONAME, "unknown node or service")                                   \
  XX(EAI_OVERFLOW, "argument buffer overflow")                                \
  XX(EAI_PROTOCOL, "resolved protocol is unknown")                            \
  XX(EAI_SERVICE, "service not available for socket type")                    \
  XX(EAI_SOCKTYPE, "socket type not supported")                               \
  XX(EALREADY, "connection already in progress")                              \
  XX(EBADF, "bad file descriptor")                                            \
  XX(EBUSY, "resource busy or locked")                                        \
  XX(ECANCELED, "operation canceled")                                         \
  XX(ECHARSET, "invalid Unicode character")                                   \
  XX(ECONNABORTED, "software caused connection abort")                        \
  XX(ECONNREFUSED, "connection refused")                                      \
  XX(ECONNRESET, "connection reset by peer")                                  \
  XX(EDESTADDRREQ, "destination address required")                            \
  XX(EEXIST, "file already exists")                                           \
  XX(EFAULT, "bad address in system call argument")                           \
  XX(EFBIG, "file too large")                                                 \
  XX(EHOSTUNREACH, "host is unreachable")                                     \
  XX(EINTR, "interrupted system call")                                        \
  XX(EINVAL, "invalid argument")                                              \
  XX(EIO, "i/o error")                                                        \
  XX(EISCONN, "socket is already connected")                                  \
  XX(EISDIR, "illegal operation on a directory")                              \
  XX(ELOOP, "too many symbolic links encountered")                            \
  XX(EMFILE, "too many open files")                                           \
  XX(EMSGSIZE, "message too long")                                            \
  XX(ENAMETOOLONG, "name too long")                                           \
  XX(ENETDOWN, "network is down")                                             \
  XX(ENETUNREACH, "network is unreachable")                                   \
  XX(ENFILE, "file table overflow")                                           \
  XX(ENOBUFS, "no buffer space available")                                    \
  XX(ENODEV, "no such device")                                                \
  XX(ENOENT, "no such file or directory")                                     \
  XX(ENOMEM, "not enough memory")                                             \
  XX(ENONET, "machine is not on the network")                                 \
  XX(ENOPROTOOPT, "protocol not available")                                   \
  XX(ENOSPC, "no space left on device")                                       \
  XX(ENOSYS, "function not implemented")                                      \
  XX(ENOTCONN, "socket is not connected")                                     \
  XX(ENOTDIR, "not a directory")                                              \
  XX(ENOTEMPTY, "directory not empty")                                        \
  XX(ENOTSOCK, "socket operation on non-socket")                              \
  XX(ENOTSUP, "operation not supported on socket")                            \
  XX(EPERM, "operation not permitted")                                        \
  XX(EPIPE, "broken pipe")                                                    \
  XX(EPROTO, "protocol error")                                                \
  XX(EPROTONOSUPPORT, "protocol not supported")                               \
  XX(EPROTOTYPE, "protocol wrong type for socket")                            \
  XX(ERANGE, "result too large")                                              \
  XX(EROFS, "read-only file system")                                          \
  XX(ESHUTDOWN, "cannot send after transport endpoint shutdown")              \
  XX(ESPIPE, "invalid seek")                                                  \
  XX(ESRCH, "no such process")                                                \
  XX(ETIMEDOUT, "connection timed out")                                       \
  XX(ETXTBSY, "text file is busy")                                            \
  XX(EXDEV, "cross-device link not permitted")                                \
  XX(UNKNOWN, "unknown error")                                                \
  XX(EOF, "end of file")                                                      \
  XX(ENXIO, "no such device or address")                                      \
  XX(EMLINK, "too many links")                                                \
  XX(EHOSTDOWN, "host is down")                                               \
  XX(EREMOTEIO, "remote I/O error")                                           \
  XX(ENOTTY, "inappropriate ioctl for device")                                \
  XX(EFTYPE, "inappropriate file type or format")                             \
  XX(EILSEQ, "illegal byte sequence")                                         \
  XX(EUNKNOWN, "unknown")

typedef enum {
#define XX(code, _) TUYA_UV_ ## code = TUYA_UV__ ## code,
  TUYA_UV_ERRNO_MAP(XX)
#undef XX
  TUYA_UV_ERRNO_MAX = TUYA_UV__EOF - 1
} tuya_uv_errno_t;

#define FD_SET_RDINDEX 0
#define FD_SET_WRINDEX 1
#define FD_SET_EXINDEX 2
#define FD_SET_NUM 3

typedef enum
{
  TUYA_UV_LOOP_BLOCK_SIGNAL
} tuya_uv_loop_option;

typedef enum
{
  TUYA_UV_RUN_DEFAULT = 0,
  TUYA_UV_RUN_ONCE,
  TUYA_UV_RUN_NOWAIT
} tuya_uv_run_mode;

typedef enum
{
  TUYA_UV_UDP = 1,
  TUYA_UV_TCP,
  TUYA_UV_TIMER,
  TUYA_UV_HANDLE_TYPE_MAX
} tuya_uv_handle_type;

typedef struct tuya_uv_loop_s tuya_uv_loop_t;
typedef struct tuya_uv_handle_s tuya_uv_handle_t;
typedef struct tuya_uv_stream_s tuya_uv_stream_t;
typedef struct tuya_uv_tcp_s tuya_uv_tcp_t;
typedef struct tuya_uv_udp_s tuya_uv_udp_t;
typedef struct tuya_uv_timer_s tuya_uv_timer_t;

/* Request types. */
typedef struct tuya_uv_req_s tuya_uv_req_t;
typedef struct tuya_uv_getaddrinfo_s tuya_uv_getaddrinfo_t;
typedef struct tuya_uv_shutdown_s tuya_uv_shutdown_t;
typedef struct tuya_uv_write_s tuya_uv_write_t;
typedef struct tuya_uv_connect_s tuya_uv_connect_t;
typedef struct tuya_uv_udp_send_s tuya_uv_udp_send_t;

typedef void (*tuya_uv_alloc_cb)(tuya_uv_handle_t *handle,
                            size_t suggested_size,
                            tuya_uv_buf_t *buf);
typedef void (*tuya_uv_read_cb)(tuya_uv_stream_t *stream,
                            ssize_t nread,
                            const tuya_uv_buf_t *buf);
typedef void (*tuya_uv_write_cb)(tuya_uv_write_t *req, int status);
typedef void (*tuya_uv_connect_cb)(tuya_uv_connect_t *req, int status);
typedef void (*tuya_uv_shutdown_cb)(tuya_uv_shutdown_t *req, int status);
typedef void (*tuya_uv_connection_cb)(tuya_uv_stream_t *server, int status);
typedef void (*tuya_uv_close_cb)(tuya_uv_handle_t *handle);
typedef void (*tuya_uv_timer_cb)(tuya_uv_timer_t *handle);
typedef void (*tuya_uv_udp_send_cb)(tuya_uv_udp_send_t *req, int status);
typedef void (*tuya_uv_udp_recv_cb)(tuya_uv_udp_t *handle,
                                ssize_t nread,
                                const tuya_uv_buf_t *buf,
                                const struct sockaddr *addr,
                                unsigned flags);

typedef void (*tuya_uv_getaddrinfo_cb)(tuya_uv_getaddrinfo_t *req,
                                  int status,
                                  struct addrinfo *res);

struct tuya_uv_loop_s {
  /* User data - use this for whatever. */
  void *data;
  /* Loop reference counting. */
  unsigned int active_handles;
  void *handle_queue[2];
  union {
    void *unused[2];
    unsigned int count;
  } active_reqs;
  int stop_flag;
  TUYA_UV_LOOP_PRIVATE_FIELDS
};

#define TUYA_UV_HANDLE_PRIVATE_FIELDS \
tuya_uv_handle_t *next_closing;     \
unsigned int flags;
#define TUYA_UV_HANDLE_FIELDS \
/* public */           \
void *data;            \
/* read-only */        \
tuya_uv_loop_t *loop;       \
int type;              \
/* private */          \
tuya_uv_close_cb close_cb;  \
void *handle_queue[2]; \
TUYA_UV_HANDLE_PRIVATE_FIELDS
/* The abstract base class of all handles. */
struct tuya_uv_handle_s {
  TUYA_UV_HANDLE_FIELDS
};
struct tuya_uv_timer_s {
  TUYA_UV_HANDLE_FIELDS
  TUYA_UV_TIMER_PRIVATE_FIELDS
};
enum tuya_uv_udp_flags {
  /* Disables dual stack mode. */
  TUYA_UV_UDP_IPV6ONLY = 1,
  /*
* Indicates message was truncated because read buffer was too small. The
* remainder was discarded by the OS. Used in tuya_uv_udp_recv_cb.
*/
  TUYA_UV_UDP_PARTIAL = 2,
  /*
* Indicates if SO_REUSEADDR will be set when binding the handle.
* This sets the SO_REUSEPORT socket flag on the BSDs and OS X. On other
* Unix platforms, it sets the SO_REUSEADDR flag.  What that means is that
* multiple threads or processes can bind to the same address without error
* (provided they all set the flag) but only the last one to bind will receive
* any traffic, in effect "stealing" the port from the previous listener.
*/
  TUYA_UV_UDP_REUSEADDR = 4
};
struct tuya_uv_udp_s {
  TUYA_UV_HANDLE_FIELDS
  size_t send_queue_size;
  size_t send_queue_count;
  TUYA_UV_UDP_PRIVATE_FIELDS
};
#define TUYA_UV_STREAM_PRIVATE_FIELDS  \
tuya_uv_connect_t *connect_req;      \
tuya_uv_shutdown_t *shutdown_req;    \
tuya_uv__io_t io_watcher;            \
void *write_queue[2];           \
void *write_completed_queue[2]; \
tuya_uv_connection_cb connection_cb; \
int delayed_error;              \
int accepted_fd;                \
void *queued_fds;               \
TUYA_UV_STREAM_PRIVATE_PLATFORM_FIELDS
#define TUYA_UV_STREAM_FIELDS                   \
/* number of bytes queued for writing */ \
size_t write_queue_size;                 \
tuya_uv_alloc_cb alloc_cb;                    \
tuya_uv_read_cb read_cb;                      \
/* private */                            \
TUYA_UV_STREAM_PRIVATE_FIELDS
/*
* tuya_uv_stream_t is a subclass of tuya_uv_handle_t.
*
* tuya_uv_stream is an abstract class.
*
* tuya_uv_stream_t is the parent class of tuya_uv_tcp_t, tuya_uv_pipe_t and tuya_uv_tty_t.
*/
struct tuya_uv_stream_s {
  TUYA_UV_HANDLE_FIELDS
  TUYA_UV_STREAM_FIELDS
};
/*
* tuya_uv_tcp_t is a subclass of tuya_uv_stream_t.
*
* Represents a TCP stream or TCP server.
*/
struct tuya_uv_tcp_s {
  TUYA_UV_HANDLE_FIELDS
  TUYA_UV_STREAM_FIELDS
  TUYA_UV_TCP_PRIVATE_FIELDS
};
enum tuya_uv_tcp_flags {
  /* Used with tuya_uv_tcp_bind, when an IPv6 address is used. */
  TUYA_UV_TCP_IPV6ONLY = 1
};

#define TUYA_UV_REQ_TYPE_MAP(XX)    \
XX(REQ, req)                 \
XX(CONNECT, connect)         \
XX(WRITE, write)             \
XX(SHUTDOWN, shutdown)       \
XX(UDP_SEND, udp_send)       \
XX(FS, fs)                   \
XX(WORK, work)               \
XX(GETADDRINFO, getaddrinfo) \
XX(GETNAMEINFO, getnameinfo)

typedef enum {
  TUYA_UV_UNKNOWN_REQ = 0,
#define XX(uc, lc) TUYA_UV_##uc,
  TUYA_UV_REQ_TYPE_MAP(XX)
#undef XX
      TUYA_UV_REQ_TYPE_MAX
} tuya_uv_req_type;
#define TUYA_UV_REQ_FIELDS \
/* public */        \
void *data;         \
/* read-only */     \
tuya_uv_req_type type;   \
/* private */       \
void *reserved[6];  \
tuya_uv_loop_t *loop;    \
TUYA_UV_REQ_PRIVATE_FIELDS
/* Abstract base class of all requests. */
struct tuya_uv_req_s {
  TUYA_UV_REQ_FIELDS
};
/* tuya_uv_udp_send_t is a subclass of tuya_uv_req_t. */
struct tuya_uv_udp_send_s {
  TUYA_UV_REQ_FIELDS
  tuya_uv_udp_t *handle;
  tuya_uv_udp_send_cb cb;
  TUYA_UV_UDP_SEND_PRIVATE_FIELDS
};
struct tuya_uv_shutdown_s {
  TUYA_UV_REQ_FIELDS
  tuya_uv_stream_t *handle;
  tuya_uv_shutdown_cb cb;
  TUYA_UV_SHUTDOWN_PRIVATE_FIELDS
};
/* tuya_uv_write_t is a subclass of tuya_uv_req_t. */
struct tuya_uv_write_s {
  TUYA_UV_REQ_FIELDS
  tuya_uv_write_cb cb;
  tuya_uv_stream_t *handle;
  TUYA_UV_WRITE_PRIVATE_FIELDS
};
/* tuya_uv_connect_t is a subclass of tuya_uv_req_t. */
struct tuya_uv_connect_s {
  TUYA_UV_REQ_FIELDS
  tuya_uv_connect_cb cb;
  tuya_uv_stream_t *handle;
  TUYA_UV_CONNECT_PRIVATE_FIELDS
};
typedef struct tuya_uv__work
{
  struct tuya_uv__work *next_work;
  void (*work)(struct tuya_uv__work *w);
  void (*done)(struct tuya_uv__work *w, int status);
  int work_no;
} tuya_uv__work_t;
/*
* tuya_uv_getaddrinfo_t is a subclass of tuya_uv_req_t.
*
* Request object for tuya_uv_getaddrinfo.
*/
struct tuya_uv_getaddrinfo_s {
  TUYA_UV_REQ_FIELDS
  /* struct addrinfo* addrinfo is marked as private, but it really isn't. */
  TUYA_UV_GETADDRINFO_PRIVATE_FIELDS
};
typedef struct tuya_uv_interface_address_s {
  char *name;
  char phys_addr[6];
  int is_internal;
  union {
    struct sockaddr_in address4;
    struct sockaddr_in6 address6;
  } address;
  union {
    struct sockaddr_in netmask4;
    struct sockaddr_in6 netmask6;
  } netmask;
} tuya_uv_interface_address_t;

typedef void *(*tuya_uv_malloc_func)(size_t size);
typedef void *(*tuya_uv_realloc_func)(void *ptr, size_t size);
typedef void *(*tuya_uv_calloc_func)(size_t count, size_t size);
typedef void (*tuya_uv_free_func)(void *ptr);

int tuya_uv_replace_allocator(tuya_uv_malloc_func malloc_func,
                          tuya_uv_realloc_func realloc_func,
                          tuya_uv_calloc_func calloc_func,
                          tuya_uv_free_func free_func);

int tuya_uv_run(tuya_uv_loop_t *, tuya_uv_run_mode mode);
tuya_uv_buf_t tuya_uv_buf_init(char *base, size_t len);
const char *tuya_uv_strerror(int err);
uint64_t tuya_uv_hrtime(void);

void tuya_uv_close(tuya_uv_handle_t *handle, tuya_uv_close_cb close_cb);
int tuya_uv_is_closing(const tuya_uv_handle_t *handle);
int tuya_uv_fileno(const tuya_uv_handle_t *handle, int *fd);
void *tuya_uv_handle_get_data(const tuya_uv_handle_t *handle);
void tuya_uv_handle_set_data(tuya_uv_handle_t *handle, void *data);

int tuya_uv_loop_init(tuya_uv_loop_t *loop);
tuya_uv_loop_t *tuya_uv_default_loop(void);
int tuya_uv_loop_close(tuya_uv_loop_t *loop);

int tuya_uv_timer_init(tuya_uv_loop_t *, tuya_uv_timer_t *handle);
int tuya_uv_timer_start(tuya_uv_timer_t *handle,
                    tuya_uv_timer_cb cb,
                    uint64_t timeout,
                    uint64_t repeat);
int tuya_uv_timer_stop(tuya_uv_timer_t *handle);

int tuya_uv_mutex_init(tuya_uv_mutex_t *handle);
void tuya_uv_mutex_destroy(tuya_uv_mutex_t *handle);
void tuya_uv_mutex_lock(tuya_uv_mutex_t *handle);
void tuya_uv_mutex_unlock(tuya_uv_mutex_t *handle);
void tuya_uv_cond_signal(tuya_uv_cond_t *cond);
int tuya_uv_cond_init(tuya_uv_cond_t *cond);
void tuya_uv_cond_destroy(tuya_uv_cond_t *cond);
int tuya_uv_cond_timedwait(tuya_uv_cond_t *cond,
                      tuya_uv_mutex_t *mutex,
                      uint64_t timeout);

int tuya_uv_ip4_addr(const char *ip, int port, struct sockaddr_in *addr);
int tuya_uv_ip4_name(const struct sockaddr_in *src, char *dst, size_t size);
int tuya_uv_ip6_name(const struct sockaddr_in6 *src, char *dst, size_t size);
int tuya_uv_ip6_addr(const char *ip, int port, struct sockaddr_in6 *addr);
int tuya_uv_inet_ntop(int af, const void *src, char *dst, size_t size);
int tuya_uv_interface_addresses(tuya_uv_interface_address_t **addresses,
                            int *count);
void tuya_uv_free_interface_addresses(tuya_uv_interface_address_t *addresses,
                                  int count);
int tuya_uv_inet_pton(int af, const char *src, void *dst);
void tuya_uv_freeaddrinfo(struct addrinfo *ai);
int tuya_uv_getaddrinfo(tuya_uv_loop_t *loop,
                    tuya_uv_getaddrinfo_t *req,
                    tuya_uv_getaddrinfo_cb getaddrinfo_cb,
                    const char *node,
                    const char *service,
                    const struct addrinfo *hints);
void tuya_uv_print_address(struct addrinfo *addrinfo, char *print_info);

int tuya_uv_cancel(tuya_uv_req_t *req);
int tuya_uv_shutdown(tuya_uv_shutdown_t *req,
                tuya_uv_stream_t *handle,
                tuya_uv_shutdown_cb cb);
size_t tuya_uv_stream_get_write_queue_size(const tuya_uv_stream_t *stream);
int tuya_uv_listen(tuya_uv_stream_t *stream, int backlog, tuya_uv_connection_cb cb);
int tuya_uv_accept(tuya_uv_stream_t *server, tuya_uv_stream_t *client);
int tuya_uv_read_start(tuya_uv_stream_t *,
                  tuya_uv_alloc_cb alloc_cb,
                  tuya_uv_read_cb read_cb);
int tuya_uv_read_stop(tuya_uv_stream_t *);
int tuya_uv_write(tuya_uv_write_t *req,
              tuya_uv_stream_t *handle,
              const tuya_uv_buf_t bufs[],
              unsigned int nbufs,
              tuya_uv_write_cb cb);
int tuya_uv_try_write(tuya_uv_stream_t *handle,
                  const tuya_uv_buf_t bufs[],
                  unsigned int nbufs);
int tuya_uv_is_readable(const tuya_uv_stream_t *handle);
int tuya_uv_is_writable(const tuya_uv_stream_t *handle);
int tuya_uv_stream_set_blocking(tuya_uv_stream_t *handle, int blocking);

int tuya_uv_tcp_init(tuya_uv_loop_t *, tuya_uv_tcp_t *handle);
int tuya_uv_tcp_init_ex(tuya_uv_loop_t *, tuya_uv_tcp_t *handle, unsigned int flags);
int tuya_uv_tcp_open(tuya_uv_tcp_t *handle, tuya_uv_os_sock_t sock);
int tuya_uv_tcp_nodelay(tuya_uv_tcp_t *handle, int enable);
int tuya_uv_tcp_keepalive(tuya_uv_tcp_t *handle,
                      int enable,
                      unsigned int delay);
int tuya_uv_tcp_simultaneous_accepts(tuya_uv_tcp_t *handle, int enable);
int tuya_uv_tcp_bind(tuya_uv_tcp_t *handle,
                const struct sockaddr *addr,
                unsigned int flags);
int tuya_uv_tcp_getsockname(const tuya_uv_tcp_t *handle,
                        struct sockaddr *name,
                        int *namelen);
int tuya_uv_tcp_getpeername(const tuya_uv_tcp_t *handle,
                        struct sockaddr *name,
                        int *namelen);
int tuya_uv_tcp_close_reset(tuya_uv_tcp_t *handle, tuya_uv_close_cb close_cb);
int tuya_uv_tcp_connect(tuya_uv_connect_t *req,
                    tuya_uv_tcp_t *handle,
                    const struct sockaddr *addr,
                    tuya_uv_connect_cb cb);

int tuya_uv_udp_init(tuya_uv_loop_t *, tuya_uv_udp_t *handle);
int tuya_uv_udp_init_ex(tuya_uv_loop_t *, tuya_uv_udp_t *handle, unsigned int flags);
int tuya_uv_udp_open(tuya_uv_udp_t *handle, tuya_uv_os_sock_t sock);
int tuya_uv_udp_bind(tuya_uv_udp_t *handle,
                const struct sockaddr *addr,
                unsigned int flags);
int tuya_uv_udp_connect(tuya_uv_udp_t *handle, const struct sockaddr *addr);
int tuya_uv_udp_getpeername(const tuya_uv_udp_t *handle,
                        struct sockaddr *name,
                        int *namelen);
int tuya_uv_udp_getsockname(const tuya_uv_udp_t *handle,
                        struct sockaddr *name,
                        int *namelen);
int tuya_uv_udp_send(tuya_uv_udp_send_t *req,
                tuya_uv_udp_t *handle,
                const tuya_uv_buf_t bufs[],
                unsigned int nbufs,
                const struct sockaddr *addr,
                tuya_uv_udp_send_cb send_cb);
int tuya_uv_udp_try_send(tuya_uv_udp_t *handle,
                    const tuya_uv_buf_t bufs[],
                    unsigned int nbufs,
                    const struct sockaddr *addr);
int tuya_uv_udp_recv_start(tuya_uv_udp_t *handle,
                      tuya_uv_alloc_cb alloc_cb,
                      tuya_uv_udp_recv_cb recv_cb);
int tuya_uv_udp_recv_stop(tuya_uv_udp_t *handle);
size_t tuya_uv_udp_get_send_queue_size(const tuya_uv_udp_t *handle);
size_t tuya_uv_udp_get_send_queue_count(const tuya_uv_udp_t *handle);

#ifdef __cplusplus
}
#endif

#endif
