/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef TUYA_UV_ERRNO_H_
#define TUYA_UV_ERRNO_H_

#include <errno.h>
#if EDOM > 0
# define TUYA_UV__ERR(x) (-(x))
#else
# define TUYA_UV__ERR(x) (x)
#endif

#define TUYA_UV__EOF     (-4095)
#define TUYA_UV__UNKNOWN (-4094)

#define TUYA_UV__EAI_ADDRFAMILY  (-3000)
#define TUYA_UV__EAI_AGAIN       (-3001)
#define TUYA_UV__EAI_BADFLAGS    (-3002)
#define TUYA_UV__EAI_CANCELED    (-3003)
#define TUYA_UV__EAI_FAIL        (-3004)
#define TUYA_UV__EAI_FAMILY      (-3005)
#define TUYA_UV__EAI_MEMORY      (-3006)
#define TUYA_UV__EAI_NODATA      (-3007)
#define TUYA_UV__EAI_NONAME      (-3008)
#define TUYA_UV__EAI_OVERFLOW    (-3009)
#define TUYA_UV__EAI_SERVICE     (-3010)
#define TUYA_UV__EAI_SOCKTYPE    (-3011)
#define TUYA_UV__EAI_BADHINTS    (-3013)
#define TUYA_UV__EAI_PROTOCOL    (-3014)

/* Only map to the system errno on non-Windows platforms. It's apparently
 * a fairly common practice for Windows programmers to redefine errno codes.
 */
#if defined(E2BIG) && !defined(_WIN32)
# define TUYA_UV__E2BIG TUYA_UV__ERR(E2BIG)
#else
# define TUYA_UV__E2BIG (-4093)
#endif

#if defined(EACCES) && !defined(_WIN32)
# define TUYA_UV__EACCES TUYA_UV__ERR(EACCES)
#else
# define TUYA_UV__EACCES (-4092)
#endif

#if defined(EADDRINUSE) && !defined(_WIN32)
# define TUYA_UV__EADDRINUSE TUYA_UV__ERR(EADDRINUSE)
#else
# define TUYA_UV__EADDRINUSE (-4091)
#endif

#if defined(EADDRNOTAVAIL) && !defined(_WIN32)
# define TUYA_UV__EADDRNOTAVAIL TUYA_UV__ERR(EADDRNOTAVAIL)
#else
# define TUYA_UV__EADDRNOTAVAIL (-4090)
#endif

#if defined(EAFNOSUPPORT) && !defined(_WIN32)
# define TUYA_UV__EAFNOSUPPORT TUYA_UV__ERR(EAFNOSUPPORT)
#else
# define TUYA_UV__EAFNOSUPPORT (-4089)
#endif

#if defined(EAGAIN) && !defined(_WIN32)
# define TUYA_UV__EAGAIN TUYA_UV__ERR(EAGAIN)
#else
# define TUYA_UV__EAGAIN (-4088)
#endif

#if defined(EALREADY) && !defined(_WIN32)
# define TUYA_UV__EALREADY TUYA_UV__ERR(EALREADY)
#else
# define TUYA_UV__EALREADY (-4084)
#endif

#if defined(EBADF) && !defined(_WIN32)
# define TUYA_UV__EBADF TUYA_UV__ERR(EBADF)
#else
# define TUYA_UV__EBADF (-4083)
#endif

#if defined(EBUSY) && !defined(_WIN32)
# define TUYA_UV__EBUSY TUYA_UV__ERR(EBUSY)
#else
# define TUYA_UV__EBUSY (-4082)
#endif

#if defined(ECANCELED) && !defined(_WIN32)
# define TUYA_UV__ECANCELED TUYA_UV__ERR(ECANCELED)
#else
# define TUYA_UV__ECANCELED (-4081)
#endif

#if defined(ECHARSET) && !defined(_WIN32)
# define TUYA_UV__ECHARSET TUYA_UV__ERR(ECHARSET)
#else
# define TUYA_UV__ECHARSET (-4080)
#endif

#if defined(ECONNABORTED) && !defined(_WIN32)
# define TUYA_UV__ECONNABORTED TUYA_UV__ERR(ECONNABORTED)
#else
# define TUYA_UV__ECONNABORTED (-4079)
#endif

#if defined(ECONNREFUSED) && !defined(_WIN32)
# define TUYA_UV__ECONNREFUSED TUYA_UV__ERR(ECONNREFUSED)
#else
# define TUYA_UV__ECONNREFUSED (-4078)
#endif

#if defined(ECONNRESET) && !defined(_WIN32)
# define TUYA_UV__ECONNRESET TUYA_UV__ERR(ECONNRESET)
#else
# define TUYA_UV__ECONNRESET (-4077)
#endif

#if defined(EDESTADDRREQ) && !defined(_WIN32)
# define TUYA_UV__EDESTADDRREQ TUYA_UV__ERR(EDESTADDRREQ)
#else
# define TUYA_UV__EDESTADDRREQ (-4076)
#endif

#if defined(EEXIST) && !defined(_WIN32)
# define TUYA_UV__EEXIST TUYA_UV__ERR(EEXIST)
#else
# define TUYA_UV__EEXIST (-4075)
#endif

#if defined(EFAULT) && !defined(_WIN32)
# define TUYA_UV__EFAULT TUYA_UV__ERR(EFAULT)
#else
# define TUYA_UV__EFAULT (-4074)
#endif

#if defined(EHOSTUNREACH) && !defined(_WIN32)
# define TUYA_UV__EHOSTUNREACH TUYA_UV__ERR(EHOSTUNREACH)
#else
# define TUYA_UV__EHOSTUNREACH (-4073)
#endif

#if defined(EINTR) && !defined(_WIN32)
# define TUYA_UV__EINTR TUYA_UV__ERR(EINTR)
#else
# define TUYA_UV__EINTR (-4072)
#endif

#if defined(EINVAL) && !defined(_WIN32)
# define TUYA_UV__EINVAL TUYA_UV__ERR(EINVAL)
#else
# define TUYA_UV__EINVAL (-4071)
#endif

#if defined(EIO) && !defined(_WIN32)
# define TUYA_UV__EIO TUYA_UV__ERR(EIO)
#else
# define TUYA_UV__EIO (-4070)
#endif

#if defined(EISCONN) && !defined(_WIN32)
# define TUYA_UV__EISCONN TUYA_UV__ERR(EISCONN)
#else
# define TUYA_UV__EISCONN (-4069)
#endif

#if defined(EISDIR) && !defined(_WIN32)
# define TUYA_UV__EISDIR TUYA_UV__ERR(EISDIR)
#else
# define TUYA_UV__EISDIR (-4068)
#endif

#if defined(ELOOP) && !defined(_WIN32)
# define TUYA_UV__ELOOP TUYA_UV__ERR(ELOOP)
#else
# define TUYA_UV__ELOOP (-4067)
#endif

#if defined(EMFILE) && !defined(_WIN32)
# define TUYA_UV__EMFILE TUYA_UV__ERR(EMFILE)
#else
# define TUYA_UV__EMFILE (-4066)
#endif

#if defined(EMSGSIZE) && !defined(_WIN32)
# define TUYA_UV__EMSGSIZE TUYA_UV__ERR(EMSGSIZE)
#else
# define TUYA_UV__EMSGSIZE (-4065)
#endif

#if defined(ENAMETOOLONG) && !defined(_WIN32)
# define TUYA_UV__ENAMETOOLONG TUYA_UV__ERR(ENAMETOOLONG)
#else
# define TUYA_UV__ENAMETOOLONG (-4064)
#endif

#if defined(ENETDOWN) && !defined(_WIN32)
# define TUYA_UV__ENETDOWN TUYA_UV__ERR(ENETDOWN)
#else
# define TUYA_UV__ENETDOWN (-4063)
#endif

#if defined(ENETUNREACH) && !defined(_WIN32)
# define TUYA_UV__ENETUNREACH TUYA_UV__ERR(ENETUNREACH)
#else
# define TUYA_UV__ENETUNREACH (-4062)
#endif

#if defined(ENFILE) && !defined(_WIN32)
# define TUYA_UV__ENFILE TUYA_UV__ERR(ENFILE)
#else
# define TUYA_UV__ENFILE (-4061)
#endif

#if defined(ENOBUFS) && !defined(_WIN32)
# define TUYA_UV__ENOBUFS TUYA_UV__ERR(ENOBUFS)
#else
# define TUYA_UV__ENOBUFS (-4060)
#endif

#if defined(ENODEV) && !defined(_WIN32)
# define TUYA_UV__ENODEV TUYA_UV__ERR(ENODEV)
#else
# define TUYA_UV__ENODEV (-4059)
#endif

#if defined(ENOENT) && !defined(_WIN32)
# define TUYA_UV__ENOENT TUYA_UV__ERR(ENOENT)
#else
# define TUYA_UV__ENOENT (-4058)
#endif

#if defined(ENOMEM) && !defined(_WIN32)
# define TUYA_UV__ENOMEM TUYA_UV__ERR(ENOMEM)
#else
# define TUYA_UV__ENOMEM (-4057)
#endif

#if defined(ENONET) && !defined(_WIN32)
# define TUYA_UV__ENONET TUYA_UV__ERR(ENONET)
#else
# define TUYA_UV__ENONET (-4056)
#endif

#if defined(ENOSPC) && !defined(_WIN32)
# define TUYA_UV__ENOSPC TUYA_UV__ERR(ENOSPC)
#else
# define TUYA_UV__ENOSPC (-4055)
#endif

#if defined(ENOSYS) && !defined(_WIN32)
# define TUYA_UV__ENOSYS TUYA_UV__ERR(ENOSYS)
#else
# define TUYA_UV__ENOSYS (-4054)
#endif

#if defined(ENOTCONN) && !defined(_WIN32)
# define TUYA_UV__ENOTCONN TUYA_UV__ERR(ENOTCONN)
#else
# define TUYA_UV__ENOTCONN (-4053)
#endif

#if defined(ENOTDIR) && !defined(_WIN32)
# define TUYA_UV__ENOTDIR TUYA_UV__ERR(ENOTDIR)
#else
# define TUYA_UV__ENOTDIR (-4052)
#endif

#if defined(ENOTEMPTY) && !defined(_WIN32)
# define TUYA_UV__ENOTEMPTY TUYA_UV__ERR(ENOTEMPTY)
#else
# define TUYA_UV__ENOTEMPTY (-4051)
#endif

#if defined(ENOTSOCK) && !defined(_WIN32)
# define TUYA_UV__ENOTSOCK TUYA_UV__ERR(ENOTSOCK)
#else
# define TUYA_UV__ENOTSOCK (-4050)
#endif

#if defined(ENOTSUP) && !defined(_WIN32)
# define TUYA_UV__ENOTSUP TUYA_UV__ERR(ENOTSUP)
#else
# define TUYA_UV__ENOTSUP (-4049)
#endif

#if defined(EPERM) && !defined(_WIN32)
# define TUYA_UV__EPERM TUYA_UV__ERR(EPERM)
#else
# define TUYA_UV__EPERM (-4048)
#endif

#if defined(EPIPE) && !defined(_WIN32)
# define TUYA_UV__EPIPE TUYA_UV__ERR(EPIPE)
#else
# define TUYA_UV__EPIPE (-4047)
#endif

#if defined(EPROTO) && !defined(_WIN32)
# define TUYA_UV__EPROTO TUYA_UV__ERR(EPROTO)
#else
# define TUYA_UV__EPROTO TUYA_UV__ERR(4046)
#endif

#if defined(EPROTONOSUPPORT) && !defined(_WIN32)
# define TUYA_UV__EPROTONOSUPPORT TUYA_UV__ERR(EPROTONOSUPPORT)
#else
# define TUYA_UV__EPROTONOSUPPORT (-4045)
#endif

#if defined(EPROTOTYPE) && !defined(_WIN32)
# define TUYA_UV__EPROTOTYPE TUYA_UV__ERR(EPROTOTYPE)
#else
# define TUYA_UV__EPROTOTYPE (-4044)
#endif

#if defined(EROFS) && !defined(_WIN32)
# define TUYA_UV__EROFS TUYA_UV__ERR(EROFS)
#else
# define TUYA_UV__EROFS (-4043)
#endif

#if defined(ESHUTDOWN) && !defined(_WIN32)
# define TUYA_UV__ESHUTDOWN TUYA_UV__ERR(ESHUTDOWN)
#else
# define TUYA_UV__ESHUTDOWN (-4042)
#endif

#if defined(ESPIPE) && !defined(_WIN32)
# define TUYA_UV__ESPIPE TUYA_UV__ERR(ESPIPE)
#else
# define TUYA_UV__ESPIPE (-4041)
#endif

#if defined(ESRCH) && !defined(_WIN32)
# define TUYA_UV__ESRCH TUYA_UV__ERR(ESRCH)
#else
# define TUYA_UV__ESRCH (-4040)
#endif

#if defined(ETIMEDOUT) && !defined(_WIN32)
# define TUYA_UV__ETIMEDOUT TUYA_UV__ERR(ETIMEDOUT)
#else
# define TUYA_UV__ETIMEDOUT (-4039)
#endif

#if defined(ETXTBSY) && !defined(_WIN32)
# define TUYA_UV__ETXTBSY TUYA_UV__ERR(ETXTBSY)
#else
# define TUYA_UV__ETXTBSY (-4038)
#endif

#if defined(EXDEV) && !defined(_WIN32)
# define TUYA_UV__EXDEV TUYA_UV__ERR(EXDEV)
#else
# define TUYA_UV__EXDEV (-4037)
#endif

#if defined(EFBIG) && !defined(_WIN32)
# define TUYA_UV__EFBIG TUYA_UV__ERR(EFBIG)
#else
# define TUYA_UV__EFBIG (-4036)
#endif

#if defined(ENOPROTOOPT) && !defined(_WIN32)
# define TUYA_UV__ENOPROTOOPT TUYA_UV__ERR(ENOPROTOOPT)
#else
# define TUYA_UV__ENOPROTOOPT (-4035)
#endif

#if defined(ERANGE) && !defined(_WIN32)
# define TUYA_UV__ERANGE TUYA_UV__ERR(ERANGE)
#else
# define TUYA_UV__ERANGE (-4034)
#endif

#if defined(ENXIO) && !defined(_WIN32)
# define TUYA_UV__ENXIO TUYA_UV__ERR(ENXIO)
#else
# define TUYA_UV__ENXIO (-4033)
#endif

#if defined(EMLINK) && !defined(_WIN32)
# define TUYA_UV__EMLINK TUYA_UV__ERR(EMLINK)
#else
# define TUYA_UV__EMLINK (-4032)
#endif

/* EHOSTDOWN is not visible on BSD-like systems when _POSIX_C_SOURCE is
 * defined. Fortunately, its value is always 64 so it's possible albeit
 * icky to hard-code it.
 */
#if defined(EHOSTDOWN) && !defined(_WIN32)
# define TUYA_UV__EHOSTDOWN TUYA_UV__ERR(EHOSTDOWN)
#elif defined(__APPLE__) || \
      defined(__DragonFly__) || \
      defined(__FreeBSD__) || \
      defined(__FreeBSD_kernel__) || \
      defined(__NetBSD__) || \
      defined(__OpenBSD__)
# define TUYA_UV__EHOSTDOWN (-64)
#else
# define TUYA_UV__EHOSTDOWN (-4031)
#endif

#if defined(EREMOTEIO) && !defined(_WIN32)
# define TUYA_UV__EREMOTEIO TUYA_UV__ERR(EREMOTEIO)
#else
# define TUYA_UV__EREMOTEIO (-4030)
#endif

#if defined(ENOTTY) && !defined(_WIN32)
# define TUYA_UV__ENOTTY TUYA_UV__ERR(ENOTTY)
#else
# define TUYA_UV__ENOTTY (-4029)
#endif

#if defined(EFTYPE) && !defined(_WIN32)
# define TUYA_UV__EFTYPE TUYA_UV__ERR(EFTYPE)
#else
# define TUYA_UV__EFTYPE (-4028)
#endif

#if defined(EILSEQ) && !defined(_WIN32)
# define TUYA_UV__EILSEQ TUYA_UV__ERR(EILSEQ)
#else
# define TUYA_UV__EILSEQ (-4027)
#endif

#if defined(EUNKNOWN) && !defined(_WIN32)
# define TUYA_UV__EUNKNOWN TUYA_UV__ERR(EUNKNOWN)
#else
# define TUYA_UV__EUNKNOWN (-4026)
#endif

#endif /* TUYA_UV_ERRNO_H_ */
