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

#ifndef __ZMDNET_PLATEFORM_H__
#define __ZMDNET_PLATEFORM_H__

#include "config.h"

#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include "../common/log.h"

#ifdef __FreeBSD__
#ifndef _SYS_MUTEX_H_
#include <sys/mutex.h>
#endif
#endif

#if !defined (_WIN32)
#define min(a,b) ((a)>(b)?(b):(a))
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#if defined(__linux__)
#define IPV6_VERSION            0x60
#endif

/* maxsockets is used in ZMDNET_ZONE_INIT call. It refers to
 * kern.ipc.maxsockets kernel environment variable.
 */
extern int maxsockets;
/* int hz; is declared in sys/kern/subr_param.c and refers to kernel timer frequency.
 * See http://ivoras.sharanet.org/freebsd/vmware.html for additional info about kern.hz
 * hz is initialized in void init_param1(void) in that file.
 */
extern int hz;
/* The following two ints define a range of available ephermal ports. */
extern int ipport_firstauto, ipport_lastauto;
/* nmbclusters is used in zmdnet_usrreq.c (e.g., zmdnet_init). In the FreeBSD kernel,
 *  this is 1024 + maxusers * 64.
 */
extern int nmbclusters;
extern int read_random(void *buf, int count); //todo puthLib into init function like zmdnet_init()
extern int ip_id;
/* necessary for zmdnet_pcb.c */
extern int ip_defttl;

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
#define u_short   unsigned __int16
#define n_time     unsigned __int32
#define sa_family_t unsigned __int8
#define ssize_t    unsigned __int64
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
#define EADDRINUSE   WSAEADDRINUSE         
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
  u_char ip_hl : 4, ip_v : 4;
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

#include "../userland/sockvar.h"
#include "../common/atomic.h"

#if defined(__FreeBSD__) && __FreeBSD_version > 602000
#include <sys/rwlock.h>
#endif

#if defined(__FreeBSD__) && __FreeBSD_version > 602000
#include <sys/priv.h>
#endif

#if defined(__DragonFly__)
/* was a 0 byte file.  needed for structs if_data(64) and net_event_data */
#include <net/if_var.h>
#endif

#if defined(__FreeBSD__)
#include <net/if_types.h>
#endif

#if !defined(_WIN32) && !defined(__native_client__)
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#endif

#if defined(HAVE_NETINET_IP_ICMP_H)
#include <netinet/ip_icmp.h>
#else
#include "../userland/icmp.h"
#endif

// #include "zmdnet-inpcb.h" // TODO
#include <limits.h>
#include <sys/types.h>

#if !defined(_WIN32)
#if defined(ZMDNET_SUPPORT_IPV4) || defined(ZMDNET_SUPPORT_IPV6)
#include <ifaddrs.h>
#endif
#include <sys/ioctl.h>
#include <unistd.h> // for close, etc.
#endif

#include <stddef.h> // for offsetof

#if defined(ZMDNET_PROCESS_LEVEL_LOCKS) && !defined(_WIN32)
/* for pthread_mutex_lock, pthread_mutex_unlock, etc. */
#include <pthread.h>
#endif

#ifdef HAVE_IPSEC
#include <netipsec/ipsec.h>
#include <netipsec/key.h>
#endif

#ifdef ZMDNET_SUPPORT_IPV6
#if defined(__FreeBSD__)
#include <sys/domain.h>
#endif

#ifdef HAVE_IPSEC
#include <netipsec/ipsec6.h>
#endif

#if !defined(_WIN32)
#include <netinet/ip6.h>
#endif

#if defined(DARWIN) || defined(__FreeBSD__) || defined(__linux__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(_WIN32)
// #include "zmdnet-ip6var.h" // TODO
#else
#include <netinet6/ip6_var.h>
#endif

#include "pcb.h"

#ifdef HAVE_SYS_QUEUE_H
#include <sys/queue.h>
#else
#include "../common/queue.h"
#endif

#if defined(__FreeBSD__)
#include <netinet6/in6_pcb.h>
#include <netinet6/ip6protosw.h>
#include <netinet6/scope6_var.h>
#endif
#endif /* INET6 */

#if defined(HAVE_PEELOFF_SOCKOPT)
#include <sys/file.h>
#include <sys/filedesc.h>
#endif

// #include "zmdnet-sha1.h" //TODO

#if __FreeBSD_version >= 700000
#include <netinet/ip_options.h>
#endif

#if defined(__FreeBSD__)
#ifndef in6pcb
#define in6pcb		inpcb
#endif
#endif

/* FIXME: temp */
#if !defined(DARWIN)
#define USER_ADDR_NULL	(NULL)		
#endif

/*
 * Local address and interface list handling todo ?????????? what is vrf????
 */
#define SCTP_MAX_VRF_ID     0
#define SCTP_SIZE_OF_VRF_HASH   3
#define SCTP_IFNAMSIZ       IFNAMSIZ
#define SCTP_DEFAULT_VRFID  0
#define SCTP_VRF_ADDR_HASH_SIZE 16
#define SCTP_VRF_IFN_HASH_SIZE  3
#define SCTP_INIT_VRF_TABLEID(vrf)

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
// use geconet typeofaddr() this is not cjeck sitelock and linklocal for ipv6
#ifdef _WIN32
#define is_ift_loopback(ifn) (strncmp((ifn)->ifn_name, "lo", 2) == 0)
#endif

/*
 * Access to IFN's to help with src-addr-selection
 */
/* This could return VOID if the index works but for BSD we provide both. */
#define get_ifn_void_from_route(ro) (void *)ro->ro_rt->rt_ifp
#define get_if_index_from_route(ro) 1 /* compiles...  TODO use routing socket to determine */
#define route_has_valid_ifn(ro) ((ro)->ro_rt && (ro)->ro_rt->rt_ifp)

#define zmdnet_malloc(mret,type,size)  do { mret = (type*) malloc(size);} while (!mret)
#define zmdnet_malloc_zero(mret,type,size)  mret = (type*) malloc(size); while (!mret) { mret = (type*) malloc(size);} memset(mret,0,size);
#define malloc_soname(mret,type,size) zmdnet_malloc_zero(mret,type,size)
#define zmdnet_free(ptr) free(ptr)

void* hash_init(int elements, u_long *hashmask);
void hash_destroy(void *vhashtbl, u_long hashmask);


// typedef struct callout timer_t which is used in the timer
// related functions such as ZMDNET_TIMER_INIT
// todo port to userspace at line 799
#include "../common/callout.h"

/*__Userspace__ defining KTR_SUBSYS 1 as done in os_macosx.h */
#define KTR_SUBSYS 1

/* The packed define for 64 bit platforms */
#if !defined(_WIN32)
#define ZMDNET_PACKED __attribute__((packed))
#define ZMDNET_UNUSED __attribute__((unused))
#else
#define ZMDNET_PACKED
#define ZMDNET_UNUSED
#endif

//todo
struct zmdnet_route
{
    int dummy;
};

struct zmdnet_rtentry
{
    int dummy;
};

static inline void zmdnet_rtalloc(struct zmdnet_route* route)
{

}
static inline void zmdnet_rtfree(struct zmdnet_rtentry* rtentry)
{

}

/*mtu*/
extern int zmdnet_get_mtu_from_ifn(uint32_t if_index, int af);
#define mtu_from_ifn_info(ifn, ifn_index, af) zmdnet_get_mtu_from_ifn(ifn_index, af)
#define mtu_from_route(ifa, sa, rt) ((rt != NULL) ? rt->rt_rmx.rmx_mtu : 0)
#define mtu_from_intfc(ifn) zmdnet_get_mtu_from_ifn(if_nametoindex(((struct ifaddrs *) (ifn))->ifa_name), AF_INET)
#define set_mtu_route(sa, rt, mtu) if(rt != NULL) rt->rt_rmx.rmx_mtu = mtu;

// TODO
// call get_inf() and then compare if it is braodcast addr
//#define is_broadcast_addr(dst, m) 0
inline unsigned char is_broadcast_addr(struct sockaddr* sa)
{
    return 1;
}

// v6 hop limit 
#define GET_HLIM(inp, ro) 128 /* As done for __Windows__ */
#define IPV6_HOP_LIMIT 128

// is the endpoint v6only? 
#define ipv6only(inp)	(((struct inpcb *)inp)->inp_flags & IN6P_IPV6_V6ONLY)

// is the socket non-blocking? 
#define is_so_nbio(so)	((so)->so_state & SS_NBIO)
#define set_sonbio(so)	((so)->so_state |= SS_NBIO)
#define clear_so_nbio(so)	((so)->so_state &= ~SS_NBIO)
#define get_so_type(so)	((so)->so_type)
// reserve sb space for a socket 
#define so_reserve(so, send, recv)	soreserve(so, send, recv)
// wakeup a socket 
#define wakeup_so(so)	wakeup(&(so)->so_timeo, so)
// clear the socket buffer state 
#define clear_so_buf(sb)	 (sb).sb_cc = 0;(sb).sb_mb = NULL;(sb).sb_mbcnt = 0;
#define get_so_recv_hiwat(so) so->so_rcv.sb_hiwat
#define get_so_snd_hiwat(so) so->so_snd.sb_hiwat
// TODO read_random
#define readrand(buf, len) read_random(buf, len)

/* start OOTB only stuff */
//An out - of - the - box feature or functionality(also called OOTB or off the shelf), particularly in software, 
//is a feature or functionality of a product that works immediately after installation without anyconfiguration 
//or modification.[1][2] It also means that it is available for all users by default, and are not required to pay 
//additionally to use those features, or needs to be configured
/* TODO IFT_LOOP is in net/if_types.h on Linux */
/* fixme not used*/
#define IFT_LOOP 0x18
#if defined(_WIN32)
#define SHUT_RD 1
#define SHUT_WR 2
#define SHUT_RDWR 3
#endif
#define PRU_FLUSH_RD SHUT_RD
#define PRU_FLUSH_WR SHUT_WR
#define PRU_FLUSH_RDWR SHUT_RDWR
#define	IP_RAWOUTPUT		0x2
/* end OOTB only stuff */

#define AF_CONN 123
struct sockaddr_conn
{
#ifdef HAVE_SCONN_LEN
  uint8_t sconn_len;
  uint8_t sconn_family;
#else
  uint16_t sconn_family;
#endif
  uint16_t sconn_port;
  void *sconn_addr;
};

#define	M_NOTIFICATION		M_PROTO5	/* zmdnet protocol  specific mbuf flags. */

/* Defining ZMDNET_IP_ID macro.
In netinet/ip_output.c, we have u_short ip_id;
In netinet/ip_var.h, we have extern u_short	ip_id; (enclosed within _KERNEL_)
See static __inline uint16_t ip_newid(void) in netinet/ip_var.h
*/
#define ZMDNET_IP_ID(inp) (ip_id)

/* need sctphdr to get port in ZMDNET_IP_OUTPUT. sctphdr defined in zmdnet.h  */
#include "zmdnet.h"
#include "ipv6_var.h"
extern void userspace_ipv4_output(int *result, struct mbuf *o_pak, struct zmdnet_route *ro, void *stcb, uint32_t vrf_id);
#if defined(ZMDNET_SUPPORT_IPV6)
extern void userspace_ipv6_output(int *result, struct mbuf *o_pak, struct route_in6 *ro, void *stcb, uint32_t vrf_id);
#endif

struct mbuf*  get_mbuf_for_msg(unsigned int space_needed, int want_header, int how, int all_in_one_buf, int type);

/* with the current included files, CMSG_ALIGN is defined in Linux but in FreeBSD, it is behind a _KERNEL in sys/socket.h ...*/
#if defined(__DragonFly__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__native_client__)
/* stolen from /usr/include/sys/socket.h */
#define CMSG_ALIGN(n)   _ALIGN(n)
#elif defined(__NetBSD__)
#define CMSG_ALIGN(n)   (((n) + __ALIGNBYTES) & ~__ALIGNBYTES)
#elif defined(DARWIN)
#if !defined(__DARWIN_ALIGNBYTES)
#define	__DARWIN_ALIGNBYTES	(sizeof(__darwin_size_t) - 1)
#endif
#if !defined(__DARWIN_ALIGN)
#define	__DARWIN_ALIGN(p)	((__darwin_size_t)((char *)(uintptr_t)(p) + __DARWIN_ALIGNBYTES) &~ __DARWIN_ALIGNBYTES)
#endif
#if !defined(__DARWIN_ALIGNBYTES32)
#define __DARWIN_ALIGNBYTES32     (sizeof(__uint32_t) - 1)
#endif
#if !defined(__DARWIN_ALIGN32)
#define __DARWIN_ALIGN32(p)       ((__darwin_size_t)((char *)(uintptr_t)(p) + __DARWIN_ALIGNBYTES32) &~ __DARWIN_ALIGNBYTES32)
#endif
#define CMSG_ALIGN(n)   __DARWIN_ALIGN32(n)
#endif

#if defined(__linux__)
#if !defined(TAILQ_FOREACH_SAFE)
#define TAILQ_FOREACH_SAFE(var, head, field, tvar)  \
for ((var) = ((head)->tqh_first); (var) && ((tvar) = TAILQ_NEXT((var), field), 1);  (var) = (tvar))
#endif
#if !defined(LIST_FOREACH_SAFE)
#define LIST_FOREACH_SAFE(var, head, field, tvar)  \
 for ((var) = ((head)->lh_first); (var) && ((tvar) = LIST_NEXT((var), field), 1); (var) = (tvar))
#endif
#endif

#if defined(__DragonFly__)
#define TAILQ_FOREACH_SAFE TAILQ_FOREACH_MUTABLE
#define LIST_FOREACH_SAFE LIST_FOREACH_MUTABLE
#endif

#ifndef timevalsub
#define timevalsub(tp1, tp2)                       \
		(tp1)->tv_sec -= (tp2)->tv_sec;    \
		(tp1)->tv_usec -= (tp2)->tv_usec;  \
		if ((tp1)->tv_usec < 0) {          \
			(tp1)->tv_sec--;           \
			(tp1)->tv_usec += 1000000; \
		} 
#endif

#if defined(__native_client__)
#define timercmp(tvp, uvp, cmp) \
(((tvp)->tv_sec == (uvp)->tv_sec) ?	 ((tvp)->tv_usec cmp (uvp)->tv_usec) : ((tvp)->tv_sec cmp (uvp)->tv_sec))
#endif

#define zmdnet_is_listening(inp) ((inp->sctp_flags & ZMDNET_PCB_FLAGS_ACCEPTING) != 0)

#endif /* SRC_SYSTEM_ZMDNET_OS_USER_SPACE_H_ */
