/*-
 * Copyright (c) 2015-2017 Mengdi Zhang
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef SRC_SYSTEM_ZMDNET_OS_USER_SPACE_H_
#define SRC_SYSTEM_ZMDNET_OS_USER_SPACE_H_


//user space includes
//All the opt_xxx.h files are placed in the kernel build directory.
//We will place them in userspace stack build directory.

#include <errno.h>
#include "zmdnet-env.h"

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <Mswsock.h>
#include <Windows.h>

typedef CRITICAL_SECTION zmdnet_mutex_t;

#if WINVER < 0x0600
enum
{
  C_SIGNAL = 0,
  C_BROADCAST = 1,
  C_MAX_EVENTS = 2
};
typedef struct
{
  u_int waiters_count;
  CRITICAL_SECTION waiters_count_lock;
  HANDLE events_[C_MAX_EVENTS];
}zmdnet_cond_t;
void InitializeXPConditionVariable(zmdnet_cond_t *);
void DeleteXPConditionVariable(zmdnet_cond_t *);
int SleepXPConditionVariable(zmdnet_cond_t *, zmdnet_mutex_t *);
void WakeAllXPConditionVariable(zmdnet_cond_t *);
#define InitializeConditionVariable(cond) InitializeXPConditionVariable(cond)
#define DeleteConditionVariable(cond) DeleteXPConditionVariable(cond)
#define SleepConditionVariableCS(cond, mtx, time) SleepXPConditionVariable(cond, mtx)
#define WakeAllConditionVariable(cond) WakeAllXPConditionVariable(cond)
#else
#define DeleteConditionVariable(cond)
typedef CONDITION_VARIABLE zmdnet_cond_t;
#endif

typedef HANDLE zmdnet_thread_t;
#define ADDRESS_FAMILY  unsigned __int8
#define IPVERSION  4
#define MAXTTL     255

/* VS2010 comes with stdint.h */
#if _MSC_VER >= 1600
#include <stdint.h>
#else
#define uint64_t   unsigned __int64
#define uint32_t   unsigned __int32
#define int32_t    __int32
#define uint16_t   unsigned __int16
#define int16_t    __int16
#define uint8_t    unsigned __int8
#define int8_t     __int8
#endif

#ifndef _SIZE_T_DEFINED
#define size_t     __int32
#endif

#define u_long     unsigned __int64
#define u_int      unsigned __int32
#define u_int32_t  unsigned __int32
#define u_int16_t  unsigned __int16
#define u_int8_t   unsigned __int8
#define u_char     unsigned char
#define n_short    unsigned __int16
#define u_short    unsigned __int16
#define n_time     unsigned __int32
#define sa_family_t unsigned __int8
#define ssize_t    __int64
#define __func__    __FUNCTION__

#ifndef EWOULDBLOCK
#define EWOULDBLOCK             WSAEWOULDBLOCK
#endif
#ifndef EINPROGRESS
#define EINPROGRESS             WSAEINPROGRESS
#endif
#ifndef EALREADY
#define EALREADY                WSAEALREADY
#endif
#ifndef ENOTSOCK
#define ENOTSOCK                WSAENOTSOCK
#endif
#ifndef EDESTADDRREQ
#define EDESTADDRREQ            WSAEDESTADDRREQ
#endif
#ifndef EMSGSIZE
#define EMSGSIZE                WSAEMSGSIZE
#endif
#ifndef EPROTOTYPE
#define EPROTOTYPE              WSAEPROTOTYPE
#endif
#ifndef ENOPROTOOPT
#define ENOPROTOOPT             WSAENOPROTOOPT
#endif
#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#endif
#ifndef ESOCKTNOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#endif
#ifndef EOPNOTSUPP
#define EOPNOTSUPP              WSAEOPNOTSUPP
#endif
#ifndef ENOTSUP
#define ENOTSUP                 WSAEOPNOTSUPP
#endif
#ifndef EPFNOSUPPORT
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#endif
#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#endif
#ifndef EADDRINUSE
#define EADDRINUSE            /* __Userspace__ version of sys/i386/include/atomic.h goes here */

/* TODO In the future, might want to not use i386 specific assembly.
 *    The options include:
 *       - implement them generically (but maybe not truly atomic?) in userspace
 *       - have ifdef's for __Userspace_arch_ perhaps (OS isn't enough...)
 */  WSAEADDRINUSE
#endif
#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#endif
#ifndef ENETDOWN
#define ENETDOWN                WSAENETDOWN
#endif
#ifndef ENETUNREACH
#define ENETUNREACH             WSAENETUNREACH
#endif
#ifndef ENETRESET
#define ENETRESET               WSAENETRESET
#endif
#ifndef ECONNABORTED
#define ECONNABORTED            WSAECONNABORTED
#endif
#ifndef ECONNRESET
#define ECONNRESET              WSAECONNRESET
#endif
#ifndef ENOBUFS
#define ENOBUFS                 WSAENOBUFS
#endif
#ifndef EISCONN
#define EISCONN                 WSAEISCONN
#endif
#ifndef ENOTCONN
#define ENOTCONN                WSAENOTCONN
#endif
#ifndef ESHUTDOWN
#define ESHUTDOWN               WSAESHUTDOWN
#endif
#ifndef ETOOMANYREFS
#define ETOOMANYREFS            WSAETOOMANYREFS
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT               WSAETIMEDOUT
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED            WSAECONNREFUSED
#endif
#ifndef ELOOP
#define ELOOP                   WSAELOOP
#endif
#ifndef EHOSTDOWN
#define EHOSTDOWN               WSAEHOSTDOWN
#endif
#ifndef EHOSTUNREACH
#define EHOSTUNREACH            WSAEHOSTUNREACH
#endif
#ifndef EPROCLIM
#define EPROCLIM                WSAEPROCLIM
#endif
#ifndef EUSERS
#define EUSERS                  WSAEUSERS
#endif
#ifndef EDQUOT
#define EDQUOT                  WSAEDQUOT
#endif
#ifndef ESTALE
#define ESTALE                  WSAESTALE
#endif
#ifndef EREMOTE
#define EREMOTE                 WSAEREMOTE
#endif

typedef char* caddr_t;

#define bzero(buf, len) memset(buf, 0, len)
#define bcopy(srcKey, dstKey, len) memcpy(dstKey, srcKey, len)

#if _MSC_VER < 1900
#define snprintf(data, size, format, ...) _snprintf_s(data, size, _TRUNCATE, format, __VA_ARGS__)
#endif
#define inline __inline
#define __inline__ __inline
#define MSG_EOR     0x8     /* data completes record */
#define MSG_DONTWAIT    0x80        /* this message should be nonblocking */

#ifdef CMSG_DATA
#undef CMSG_DATA
#endif
/*
 * The following definitions should apply iff WINVER < 0x0600
 * but that check doesn't work in all cases. So be more pedantic...
 */
#define CMSG_DATA(x) WSA_CMSG_DATA(x)
#define CMSG_ALIGN(x) WSA_CMSGDATA_ALIGN(x)
#ifndef CMSG_FIRSTHDR
#define CMSG_FIRSTHDR(x) WSA_CMSG_FIRSTHDR(x)
#endif
#ifndef CMSG_NXTHDR
#define CMSG_NXTHDR(x, y) WSA_CMSG_NXTHDR(x, y)
#endif
#ifndef CMSG_SPACE
#define CMSG_SPACE(x) WSA_CMSG_SPACE(x)
#endif
#ifndef CMSG_LEN
#define CMSG_LEN(x) WSA_CMSG_LEN(x)
#endif

/****  from sctp_os_windows.h ***************/
#define ZMDNET_IFN_IS_IFT_LOOP(ifn)   ((ifn)->ifn_type == IFT_LOOP)
#define ZMDNET_ROUTE_IS_REAL_LOOP(ro) \
((ro)->ro_rt && (ro)->ro_rt->rt_ifa && (ro)->ro_rt->rt_ifa->ifa_ifp && \
(ro)->ro_rt->rt_ifa->ifa_ifp->if_type == IFT_LOOP)

/*
 * Access to IFN's to help with src-addr-selection
 */
/* This could return VOID if the index works but for BSD we provide both. */
#define ZMDNET_GET_IFN_VOID_FROM_ROUTE(ro) \
    ((ro)->ro_rt != NULL ? (ro)->ro_rt->rt_ifp : NULL)
#define ZMDNET_ROUTE_HAS_VALID_IFN(ro) \
    ((ro)->ro_rt && (ro)->ro_rt->rt_ifp)
/******************************************/

#define ZMDNET_GET_IF_INDEX_FROM_ROUTE(ro) 1 /* compiles...  TODO use routing socket to determine */

#define BIG_ENDIAN 1
#define LITTLE_ENDIAN 0
#ifdef WORDS_BIGENDIAN
#define BYTE_ORDER BIG_ENDIAN
#else
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#else /* !defined(_WIN32) */

#include <sys/socket.h>
#if defined(__DragonFly__) || defined(__FreeBSD__) || defined(__linux__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__native_client__) || defined(__Fuchsia__)
#include <pthread.h>
#endif
typedef pthread_mutex_t zmdnet_mutex_t;
typedef pthread_cond_t zmdnet_cond_t;
typedef pthread_t zmdnet_thread_t;
#endif

#if defined(_WIN32) || defined(__native_client__)
#define IFNAMSIZ 64
#define random() rand()
#define srandom(s) srand(s)
#define timeradd(tvp, uvp, vvp)   \
    do {                          \
        (vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;  \
        (vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;  \
        if ((vvp)->tv_usec >= 1000000) {                   \
            (vvp)->tv_sec++;                        \
            (vvp)->tv_usec -= 1000000;             \
        }                         \
    } while (0)
#define timersub(tvp, uvp, vvp)   \
    do {                          \
        (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;  \
        (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;  \
        if ((vvp)->tv_usec < 0) {                   \
            (vvp)->tv_sec--;                        \
            (vvp)->tv_usec += 1000000;             \
        }                       \
    } while (0)

struct ip
{
  u_char ip_hl:4, ip_v:4;
  u_char ip_tos;
  u_short ip_len;
  u_short ip_id;
  u_short ip_off;
#define IP_RP 0x8000
#define IP_DF 0x4000
#define IP_MF 0x2000
#define IP_OFFMASK 0x1fff
  u_char ip_ttl;
  u_char ip_p;
  u_short ip_sum;
  struct in_addr ip_src, ip_dst;
};

struct ifaddrs
{
  struct ifaddrs *ifa_next;
  char *ifa_name;
  unsigned int ifa_flags;
  struct sockaddr *ifa_addr;
  struct sockaddr *ifa_netmask;
  struct sockaddr *ifa_dstaddr;
  void *ifa_data;
};

struct udphdr
{
  uint16_t uh_sport;
  uint16_t uh_dport;
  uint16_t uh_ulen;
  uint16_t uh_sum;
};

struct iovec
{
  size_t len;
  char *buf;
};

#define iov_base buf
#define iov_len len

struct ifa_msghdr
{
  uint16_t ifam_msglen;
  unsigned char ifam_version;
  unsigned char ifam_type;
  uint32_t ifam_addrs;
  uint32_t ifam_flags;
  uint16_t ifam_index;
  uint32_t ifam_metric;
};

struct ifdevmtu
{
  int ifdm_current;
  int ifdm_min;
  int ifdm_max;
};

struct ifkpi
{
  unsigned int ifk_module_id;
  unsigned int ifk_type;
  union
  {
    void *ifk_ptr;
    int ifk_value;
  }ifk_data;
};

struct ifreq
{
  char ifr_name[16];
  union
  {
    struct sockaddr ifru_addr;
    struct sockaddr ifru_dstaddr;
    struct sockaddr ifru_broadaddr;
    short ifru_flags;
    int ifru_metric;
    int ifru_mtu;
    int ifru_phys;
    int ifru_media;
    int ifru_intval;
    char* ifru_data;
    struct ifdevmtu ifru_devmtu;
    struct ifkpi ifru_kpi;
    uint32_t ifru_wake_flags;
  }ifr_ifru;
#define ifr_addr        ifr_ifru.ifru_addr
#define ifr_dstaddr     ifr_ifru.ifru_dstaddr
#define ifr_broadaddr   ifr_ifru.ifru_broadaddr
#define ifr_flags       ifr_ifru.ifru_flags[0]
#define ifr_prevflags   ifr_ifru.ifru_flags[1]
#define ifr_metric      ifr_ifru.ifru_metric
#define ifr_mtu         ifr_ifru.ifru_mtu
#define ifr_phys        ifr_ifru.ifru_phys
#define ifr_media       ifr_ifru.ifru_media
#define ifr_data        ifr_ifru.ifru_data
#define ifr_devmtu      ifr_ifru.ifru_devmtu
#define ifr_intval      ifr_ifru.ifru_intval
#define ifr_kpi         ifr_ifru.ifru_kpi
#define ifr_wake_flags  ifr_ifru.ifru_wake_flags
};
#endif

#if defined(_WIN32)
int Win_getifaddrs(struct ifaddrs**);
#define getifaddrs(interfaces)  (int)Win_getifaddrs(interfaces)
int win_if_nametoindex(const char *);
#define if_nametoindex(x) win_if_nametoindex(x)
#endif

#if !defined(__FreeBSD__)
struct mtx
{
  int dummy;
};
struct sx
{
  int dummy;
};
#if !defined(__NetBSD__)
struct selinfo
{
  int dummy;
};
#endif
#endif

#include <stdio.h>
#include <string.h>
/* #include <sys/param.h>  in FreeBSD defines MSIZE */
/* #include <sys/ktr.h> */
/* #include <sys/systm.h> */
#if defined(HAVE_SYS_QUEUE_H)
#include <sys/queue.h>
#else
#include "zmdnet-queue.h"
#endif

#include "zmdnet-atomic.h"

//IAMHERE   sctp_os_userspace.h line 462 #include "user_socketvar.h"

#endif /* SRC_SYSTEM_ZMDNET_OS_USER_SPACE_H_ */