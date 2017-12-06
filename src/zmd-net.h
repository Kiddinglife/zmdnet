#ifndef __ZMDNET_H__
#define __ZMDNET_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <sys/types.h>

#ifdef _WIN32
#ifdef _MSC_VER
#pragma warning(disable: 4200)
#endif
#include <winsock2.h> // socket operations receive send listen accept and so on
#include <ws2tcpip.h> // inet addr defs
#else
#include <sys/socket.h> // socket operations receive send listen accept and so on
#include <netinet/in.h> // inet addr defs
#endif

#ifndef MSG_NOTIFICATION
/* This definition MUST be in sync with zmd-net-socketvar.h */
#define MSG_NOTIFICATION 0x2000
#endif

#ifndef IPPROTO_ZMD_NET
/* protocol number of zmdnet. */
#define IPPROTO_ZMD_NET 135
#endif

#ifdef _WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1600
#include <stdint.h>
#elif defined(ZMDNET_STDINT_INCLUDE)
#include ZMDNET_STDINT_INCLUDE
#else
#define uint8_t   unsigned __int8
#define uint16_t  unsigned __int16
#define uint32_t  unsigned __int32
#define uint64_t  unsigned __int64
#define int16_t   __int16
#define int32_t   __int32
#endif

#define ssize_t   __int64
#define MSG_EOR   0x8
#ifndef EWOULDBLOCK
#define EWOULDBLOCK  WSAEWOULDBLOCK
#endif
#ifndef EINPROGRESS
#define EINPROGRESS  WSAEINPROGRESS
#endif
#define SHUT_RD    1
#define SHUT_WR    2
#define SHUT_RDWR  3
#endif

#define ZMDNET_FUTURE_ASSOC  0
#define ZMDNET_CURRENT_ASSOC 1
#define ZMDNET_ALL_ASSOC     2

typedef uint32_t zmdnet_assoc_t;

#if defined(_WIN32) && defined(_MSC_VER)
#pragma pack (push, 1)
#define ZMDNET_PACKED
#else
#define ZMDNET_PACKED __attribute__((packed))
#endif
struct zmdnet_common_header {
	uint16_t source_port;
	uint16_t destination_port;
	uint32_t verification_tag;
	uint32_t crc32c;
}ZMDNET_PACKED;
#if defined(_WIN32) && defined(_MSC_VER)
#pragma pack(pop)
#endif
#undef ZMDNET_PACKED

/* The definition of struct sockaddr_conn MUST be in
 * tune with other sockaddr_* structures.
 */
#define AF_CONN 123
#if defined(__APPLE__) || defined(__Bitrig__) || defined(__DragonFly__) || \
    defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
struct sockaddr_conn {
	uint8_t sconn_len;
	uint8_t sconn_family;
	uint16_t sconn_port;
	void *sconn_addr;
};
#else
struct sockaddr_conn {
	uint16_t sconn_family;
	uint16_t sconn_port;
	void *sconn_addr;
};
#endif

union zmdnet_sockaddr_store {
#if defined(INET)
	struct sockaddr_in sin;
#endif
#if defined(INET6)
	struct sockaddr_in6 sin6;
#endif
	struct sockaddr_conn sconn;
	struct sockaddr sa;
};

/*** { Structures and definitions to use the socket API ***/
#define ZMDNET_ALIGN_RESV_PAD 92
#define ZMDNET_ALIGN_RESV_PAD_SHORT 76

#define ZMDNET_NO_NEXT_MSG           0x0000
#define ZMDNET_NEXT_MSG_AVAIL        0x0001
#define ZMDNET_NEXT_MSG_ISCOMPLETE   0x0002
#define ZMDNET_NEXT_MSG_IS_UNORDERED 0x0004
#define ZMDNET_NEXT_MSG_IS_NOTIFICATION 0x0008

#define ZMDNET_RECVV_NOINFO  0
#define ZMDNET_RECVV_RCVINFO 1
#define ZMDNET_RECVV_NXTINFO 2
#define ZMDNET_RECVV_RN      3

#define ZMDNET_SENDV_NOINFO   0
#define ZMDNET_SENDV_SNDINFO  1
#define ZMDNET_SENDV_PRINFO   2
#define ZMDNET_SENDV_AUTHINFO 3
#define ZMDNET_SENDV_SPA      4

#define ZMDNET_SEND_SNDINFO_VALID  0x00000001
#define ZMDNET_SEND_PRINFO_VALID   0x00000002
#define ZMDNET_SEND_AUTHINFO_VALID 0x00000004

struct zmdnet_rcvinfo {
	uint16_t sid;
	uint16_t ssn;
	uint16_t flags;
	uint32_t ppid;
	uint32_t tsn;
	uint32_t cumtsn;
	uint32_t context;
	zmdnet_assoc_t associd;
};

struct zmdnet_nxtinfo {
	uint16_t sid;
	uint16_t flags;
	uint32_t ppid;
	uint32_t length;
	zmdnet_assoc_t assoc_id;
};

struct zmdnet_recvv_rn {
	struct zmdnet_rcvinfo rcvinfo;
	struct zmdnet_nxtinfo nxtinfo;
};

struct zmdnet_snd_all_completes {
	uint16_t sall_stream;
	uint16_t sall_flags;
	uint32_t sall_ppid;
	uint32_t sall_context;
	uint32_t sall_num_sent;
	uint32_t sall_num_failed;
};

struct zmdnet_sndinfo {
	uint16_t snd_sid;
	uint16_t snd_flags;
	uint32_t snd_ppid;
	uint32_t snd_context;
	zmdnet_assoc_t snd_assoc_id;
};

struct zmdnet_prinfo {
	uint16_t pr_policy;
	uint32_t pr_value;
};

struct zmdnet_authinfo {
	uint16_t auth_keynumber;
};

struct zmdnet_sendv_spa {
	uint32_t sendv_flags;
	struct zmdnet_sndinfo sendv_sndinfo;
	struct zmdnet_prinfo sendv_prinfo;
	struct zmdnet_authinfo sendv_authinfo;
};

struct zmdnet_udpencaps {
	struct sockaddr_storage sue_address;
	uint32_t sue_assoc_id;
	uint16_t sue_port;
};

struct zmdnet_udp_encaps {
	struct sockaddr_storage sue_address;
	uint32_t sue_assoc_id;
	uint16_t sue_port;
};
/***  Structures and definitions to use the socket API  } ***/



/********  { Notifications  **************/

/* notification types */
#define ZMDNET_ASSOC_CHANGE                 0x0001
#define ZMDNET_PEER_ADDR_CHANGE             0x0002
#define ZMDNET_REMOTE_ERROR                 0x0003
#define ZMDNET_SEND_FAILED                  0x0004
#define ZMDNET_SHUTDOWN_EVENT               0x0005
#define ZMDNET_ADAPTATION_INDICATION        0x0006
#define ZMDNET_PARTIAL_DELIVERY_EVENT       0x0007
#define ZMDNET_AUTHENTICATION_EVENT         0x0008
#define ZMDNET_STREAM_RESET_EVENT           0x0009
#define ZMDNET_SENDER_DRY_EVENT             0x000a
#define ZMDNET_NOTIFICATIONS_STOPPED_EVENT  0x000b
#define ZMDNET_ASSOC_RESET_EVENT            0x000c
#define ZMDNET_STREAM_CHANGE_EVENT          0x000d
#define ZMDNET_SEND_FAILED_EVENT            0x000e

/* sac_state values */
#define ZMDNET_COMM_UP        0x0001
#define ZMDNET_COMM_LOST      0x0002
#define ZMDNET_RESTART        0x0003
#define ZMDNET_SHUTDOWN_COMP  0x0004
#define ZMDNET_CANT_STR_ASSOC 0x0005

/* sac_info values */
#define ZMDNET_ASSOC_SUPPORTS_PR        0x01
#define ZMDNET_ASSOC_SUPPORTS_AUTH      0x02
#define ZMDNET_ASSOC_SUPPORTS_ASCONF    0x03
#define ZMDNET_ASSOC_SUPPORTS_MULTIBUF  0x04
#define ZMDNET_ASSOC_SUPPORTS_RE_CONFIG 0x05
#define ZMDNET_ASSOC_SUPPORTS_MAX       0x05

/* paddr state values */
#define ZMDNET_ADDR_AVAILABLE   0x0001
#define ZMDNET_ADDR_UNREACHABLE 0x0002
#define ZMDNET_ADDR_REMOVED     0x0003
#define ZMDNET_ADDR_ADDED       0x0004
#define ZMDNET_ADDR_MADE_PRIM   0x0005
#define ZMDNET_ADDR_CONFIRMED   0x0006

/* indication values */
#define ZMDNET_PARTIAL_DELIVERY_ABORTED  0x0001

#define ZMDNET_AUTH_NEW_KEY   0x0001
#define ZMDNET_AUTH_NO_AUTH   0x0002
#define ZMDNET_AUTH_FREE_KEY  0x0003

/* flags in stream_reset_event (strreset_flags) */
#define ZMDNET_STREAM_RESET_INCOMING_SSN  0x0001
#define ZMDNET_STREAM_RESET_OUTGOING_SSN  0x0002
#define ZMDNET_STREAM_RESET_DENIED        0x0004 /* ZMDNET_STRRESET_FAILED */
#define ZMDNET_STREAM_RESET_FAILED        0x0008 /* ZMDNET_STRRESET_FAILED */
#define ZMDNET_STREAM_CHANGED_DENIED      0x0010
#define ZMDNET_STREAM_RESET_INCOMING      0x00000001
#define ZMDNET_STREAM_RESET_OUTGOING      0x00000002

#define ZMDNET_STREAM_CHANGE_DENIED	0x0004
#define ZMDNET_STREAM_CHANGE_FAILED	0x0008

#define ZMDNET_ASSOC_RESET_DENIED        0x0004
#define ZMDNET_ASSOC_RESET_FAILED        0x0008

/* Assoc reset event - subscribe to ZMDNET_ASSOC_RESET_EVENT */
struct zmdnet_assoc_reset_event {
	uint16_t assocreset_type;
	uint16_t assocreset_flags;
	uint32_t assocreset_length;
	zmdnet_assoc_t assocreset_assoc_id;
	uint32_t assocreset_local_tsn;
	uint32_t assocreset_remote_tsn;
};

/* Stream change event - subscribe to ZMDNET_STREAM_CHANGE_EVENT */
struct zmdnet_stream_change_event {
	uint16_t strchange_type;
	uint16_t strchange_flags;
	uint32_t strchange_length;
	zmdnet_assoc_t strchange_assoc_id;
	uint16_t strchange_instrms;
	uint16_t strchange_outstrms;
};

/* Stream reset event - subscribe to ZMDNET_STREAM_RESET_EVENT */
struct zmdnet_stream_reset_event {
	uint16_t strreset_type;
	uint16_t strreset_flags;
	uint32_t strreset_length;
	zmdnet_assoc_t strreset_assoc_id;
	uint16_t strreset_stream_list[];
};

/* ZMDNET sender dry event */
struct zmdnet_sender_dry_event {
	uint16_t sender_dry_type;
	uint16_t sender_dry_flags;
	uint32_t sender_dry_length;
	zmdnet_assoc_t sender_dry_assoc_id;
};

/* remote error events */
struct zmdnet_remote_error {
	uint16_t sre_type;
	uint16_t sre_flags;
	uint32_t sre_length;
	uint16_t sre_error;
	zmdnet_assoc_t sre_assoc_id;
	uint8_t sre_data[4];
};

/* shutdown event */
struct zmdnet_shutdown_event {
	uint16_t sse_type;
	uint16_t sse_flags;
	uint32_t sse_length;
	zmdnet_assoc_t sse_assoc_id;
};

/* Adaptation layer indication */
struct zmdnet_adaptation_event {
	uint16_t sai_type;
	uint16_t sai_flags;
	uint32_t sai_length;
	uint32_t sai_adaptation_ind;
	zmdnet_assoc_t sai_assoc_id;
};

/* Partial delivery event */
struct zmdnet_pdapi_event {
	uint16_t pdapi_type;
	uint16_t pdapi_flags;
	uint32_t pdapi_length;
	uint32_t pdapi_indication;
	uint32_t pdapi_stream;
	uint32_t pdapi_seq;
	zmdnet_assoc_t pdapi_assoc_id;
};

/* Address event */
struct zmdnet_paddr_change {
	uint16_t spc_type;
	uint16_t spc_flags;
	uint32_t spc_length;
	struct sockaddr_storage spc_aaddr;
	uint32_t spc_state;
	uint32_t spc_error;
	zmdnet_assoc_t spc_assoc_id;
	uint8_t spc_padding[4];
}

// continue from line 400 in file usrzmdnet.h
/********  Notifications  } **************/


#ifdef _WIN32
#ifdef _MSC_VER
#pragma warning(default: 4200)
#endif
#endif

#ifdef  __cplusplus
}
#endif
#endif
