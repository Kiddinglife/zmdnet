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

#ifndef __ZMDNET_H__
#define __ZMDNET_H__

#ifdef  __cplusplus
extern "C"
  {
#endif

#include <errno.h>
#include <sys/types.h>

#ifdef _WIN32
#ifdef _MSC_VER
#pragma warning(disable: 4200)
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifndef MSG_NOTIFICATION
/* This definition MUST be in sync with zmdnetlib/user_socketvar.h */
#define MSG_NOTIFICATION 0x2000
#endif

#ifndef IPPROTO_ZMDNET
/* This is the IANA assigned protocol number of ZMDNET. */
#define IPPROTO_ZMDNET 132
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

typedef uint32_t zmdnet_assoc_t;

#if defined(_WIN32) && defined(_MSC_VER)
#pragma pack (push, 1)
#define ZMDNET_PACKED
#else
#define ZMDNET_PACKED __attribute__((packed))
#endif

struct zmdnet_common_header
{
  uint16_t source_port;
  uint16_t destination_port;
  uint32_t verification_tag;
  uint32_t crc32c;
}ZMDNET_PACKED;

#if defined(_WIN32) && defined(_MSC_VER)
#pragma pack(pop)
#endif
#undef ZMDNET_PACKED

#define AF_CONN 123
/* The definition of struct sockaddr_conn MUST be in
 * tune with other sockaddr_* structures.
 */
#if defined(__APPLE__) || defined(__Bitrig__) || defined(__DragonFly__) || \
    defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
struct sockaddr_conn
  {
    uint8_t sconn_len;
    uint8_t sconn_family;
    uint16_t sconn_port;
    void *sconn_addr;
  };
#else
struct sockaddr_conn
{
  uint16_t sconn_family;
  uint16_t sconn_port;
  void *sconn_addr;
};
#endif

union zmdnet_sockstore
{
#if defined(INET)
  struct sockaddr_in sin;
#endif
#if defined(INET6)
  struct sockaddr_in6 sin6;
#endif
  struct sockaddr_conn sconn;
  struct sockaddr sa;
};

#define ZMDNET_FUTURE_ASSOC  0
#define ZMDNET_CURRENT_ASSOC 1
#define ZMDNET_ALL_ASSOC     2

/***  Structures and definitions to use the socket API  ***/

#define ZMDNET_ALIGN_RESV_PAD 92
#define ZMDNET_ALIGN_RESV_PAD_SHORT 76

struct zmdnet_rcvinfo
{
  uint16_t rcv_sid;
  uint16_t rcv_ssn;
  uint16_t rcv_flags;
  uint32_t rcv_ppid;
  uint32_t rcv_tsn;
  uint32_t rcv_cumtsn;
  uint32_t rcv_context;
  zmdnet_assoc_t rcv_assoc_id;
};

struct zmdnet_nxtinfo
{
  uint16_t nxt_sid;
  uint16_t nxt_flags;
  uint32_t nxt_ppid;
  uint32_t nxt_length;
  zmdnet_assoc_t nxt_assoc_id;
};

#define ZMDNET_NO_NEXT_MSG           0x0000
#define ZMDNET_NEXT_MSG_AVAIL        0x0001
#define ZMDNET_NEXT_MSG_ISCOMPLETE   0x0002
#define ZMDNET_NEXT_MSG_IS_UNORDERED 0x0004
#define ZMDNET_NEXT_MSG_IS_NOTIFICATION 0x0008

struct zmdnet_recvv_rn
{
  struct zmdnet_rcvinfo recvv_rcvinfo;
  struct zmdnet_nxtinfo recvv_nxtinfo;
};

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

struct zmdnet_snd_all_completes
{
  uint16_t sall_stream;
  uint16_t sall_flags;
  uint32_t sall_ppid;
  uint32_t sall_context;
  uint32_t sall_num_sent;
  uint32_t sall_num_failed;
};

struct zmdnet_sndinfo
{
  uint16_t snd_sid;
  uint16_t snd_flags;
  uint32_t snd_ppid;
  uint32_t snd_context;
  zmdnet_assoc_t snd_assoc_id;
};

struct zmdnet_prinfo
{
  uint16_t pr_policy;
  uint32_t pr_value;
};

struct zmdnet_authinfo
{
  uint16_t auth_keynumber;
};

struct zmdnet_sendv_spa
{
  uint32_t sendv_flags;
  struct zmdnet_sndinfo sendv_sndinfo;
  struct zmdnet_prinfo sendv_prinfo;
  struct zmdnet_authinfo sendv_authinfo;
};

struct zmdnet_udpencaps
{
  struct sockaddr_storage sue_address;
  uint32_t sue_assoc_id;
  uint16_t sue_port;
};

/********  Notifications  **************/

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

/* notification event structures */

/* association change event */
struct zmdnet_assoc_change
{
  uint16_t sac_type;
  uint16_t sac_flags;
  uint32_t sac_length;
  uint16_t sac_state;
  uint16_t sac_error;
  uint16_t sac_outbound_streams;
  uint16_t sac_inbound_streams;
  zmdnet_assoc_t sac_assoc_id;
  uint8_t sac_info[]; /* not available yet */
};

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

/* Address event */
struct zmdnet_paddr_change
{
  uint16_t spc_type;
  uint16_t spc_flags;
  uint32_t spc_length;
  struct sockaddr_storage spc_aaddr;
  uint32_t spc_state;
  uint32_t spc_error;
  zmdnet_assoc_t spc_assoc_id;
  uint8_t spc_padding[4];
};

/* paddr state values */
#define ZMDNET_ADDR_AVAILABLE   0x0001
#define ZMDNET_ADDR_UNREACHABLE 0x0002
#define ZMDNET_ADDR_REMOVED     0x0003
#define ZMDNET_ADDR_ADDED       0x0004
#define ZMDNET_ADDR_MADE_PRIM   0x0005
#define ZMDNET_ADDR_CONFIRMED   0x0006

/* remote error events */
struct zmdnet_remote_error
{
  uint16_t sre_type;
  uint16_t sre_flags;
  uint32_t sre_length;
  uint16_t sre_error;
  zmdnet_assoc_t sre_assoc_id;
  uint8_t sre_data[4];
};

/* shutdown event */
struct zmdnet_shutdown_event
{
  uint16_t sse_type;
  uint16_t sse_flags;
  uint32_t sse_length;
  zmdnet_assoc_t sse_assoc_id;
};

/* Adaptation layer indication */
struct zmdnet_adaptation_event
{
  uint16_t sai_type;
  uint16_t sai_flags;
  uint32_t sai_length;
  uint32_t sai_adaptation_ind;
  zmdnet_assoc_t sai_assoc_id;
};

/* Partial delivery event */
struct zmdnet_pdapi_event
{
  uint16_t pdapi_type;
  uint16_t pdapi_flags;
  uint32_t pdapi_length;
  uint32_t pdapi_indication;
  uint32_t pdapi_stream;
  uint32_t pdapi_seq;
  zmdnet_assoc_t pdapi_assoc_id;
};

/* indication values */
#define ZMDNET_PARTIAL_DELIVERY_ABORTED  0x0001

/* ZMDNET authentication event */
struct zmdnet_authkey_event
{
  uint16_t auth_type;
  uint16_t auth_flags;
  uint32_t auth_length;
  uint16_t auth_keynumber;
  uint32_t auth_indication;
  zmdnet_assoc_t auth_assoc_id;
};

/* indication values */
#define ZMDNET_AUTH_NEW_KEY   0x0001
#define ZMDNET_AUTH_NO_AUTH   0x0002
#define ZMDNET_AUTH_FREE_KEY  0x0003

/* ZMDNET sender dry event */
struct zmdnet_sender_dry_event
{
  uint16_t sender_dry_type;
  uint16_t sender_dry_flags;
  uint32_t sender_dry_length;
  zmdnet_assoc_t sender_dry_assoc_id;
};

/* Stream reset event - subscribe to ZMDNET_STREAM_RESET_EVENT */
struct zmdnet_stream_reset_event
{
  uint16_t strreset_type;
  uint16_t strreset_flags;
  uint32_t strreset_length;
  zmdnet_assoc_t strreset_assoc_id;
  uint16_t strreset_stream_list[];
};

/* flags in stream_reset_event (strreset_flags) */
#define ZMDNET_STREAM_RESET_INCOMING_SSN  0x0001
#define ZMDNET_STREAM_RESET_OUTGOING_SSN  0x0002
#define ZMDNET_STREAM_RESET_DENIED        0x0004 /* ZMDNET_STRRESET_FAILED */
#define ZMDNET_STREAM_RESET_FAILED        0x0008 /* ZMDNET_STRRESET_FAILED */
#define ZMDNET_STREAM_CHANGED_DENIED      0x0010

#define ZMDNET_STREAM_RESET_INCOMING      0x00000001
#define ZMDNET_STREAM_RESET_OUTGOING      0x00000002

/* Assoc reset event - subscribe to ZMDNET_ASSOC_RESET_EVENT */
struct zmdnet_assoc_reset_event
{
  uint16_t assocreset_type;
  uint16_t assocreset_flags;
  uint32_t assocreset_length;
  zmdnet_assoc_t assocreset_assoc_id;
  uint32_t assocreset_local_tsn;
  uint32_t assocreset_remote_tsn;
};

#define ZMDNET_ASSOC_RESET_DENIED        0x0004
#define ZMDNET_ASSOC_RESET_FAILED        0x0008

/* Stream change event - subscribe to ZMDNET_STREAM_CHANGE_EVENT */
struct zmdnet_stream_change_event
{
  uint16_t strchange_type;
  uint16_t strchange_flags;
  uint32_t strchange_length;
  zmdnet_assoc_t strchange_assoc_id;
  uint16_t strchange_instrms;
  uint16_t strchange_outstrms;
};

#define ZMDNET_STREAM_CHANGE_DENIED	0x0004
#define ZMDNET_STREAM_CHANGE_FAILED	0x0008

/* ZMDNET send failed event */
struct zmdnet_send_failed_event
{
  uint16_t ssfe_type;
  uint16_t ssfe_flags;
  uint32_t ssfe_length;
  uint32_t ssfe_error;
  struct zmdnet_sndinfo ssfe_info;
  zmdnet_assoc_t ssfe_assoc_id;
  uint8_t ssfe_data[];
};

/* flag that indicates state of data */
#define ZMDNET_DATA_UNSENT  0x0001	/* inqueue never on wire */
#define ZMDNET_DATA_SENT    0x0002	/* on wire at failure */

/* ZMDNET event option */
struct zmdnet_event
{
  zmdnet_assoc_t se_assoc_id;
  uint16_t se_type;
  uint8_t se_on;
};

union zmdnet_notification
{
  struct zmdnet_tlv
  {
    uint16_t sn_type;
    uint16_t sn_flags;
    uint32_t sn_length;
  } sn_header;
  struct zmdnet_assoc_change sn_assoc_change;
  struct zmdnet_paddr_change sn_paddr_change;
  struct zmdnet_remote_error sn_remote_error;
  struct zmdnet_shutdown_event sn_shutdown_event;
  struct zmdnet_adaptation_event sn_adaptation_event;
  struct zmdnet_pdapi_event sn_pdapi_event;
  struct zmdnet_authkey_event sn_auth_event;
  struct zmdnet_sender_dry_event sn_sender_dry_event;
  struct zmdnet_send_failed_event sn_send_failed_event;
  struct zmdnet_stream_reset_event sn_strreset_event;
  struct zmdnet_assoc_reset_event sn_assocreset_event;
  struct zmdnet_stream_change_event sn_strchange_event;
};

struct zmdnet_event_subscribe
{
  uint8_t zmdnet_data_io_event;
  uint8_t zmdnet_association_event;
  uint8_t zmdnet_address_event;
  uint8_t zmdnet_send_failure_event;
  uint8_t zmdnet_peer_error_event;
  uint8_t zmdnet_shutdown_event;
  uint8_t zmdnet_partial_delivery_event;
  uint8_t zmdnet_adaptation_layer_event;
  uint8_t zmdnet_authentication_event;
  uint8_t zmdnet_sender_dry_event;
  uint8_t zmdnet_stream_reset_event;
};

/* Flags that go into the sinfo->sinfo_flags field */
#define ZMDNET_NOTIFICATION     0x0010 /* next message is a notification */
#define ZMDNET_COMPLETE         0x0020 /* next message is complete */
#define ZMDNET_EOF              0x0100 /* Start shutdown procedures */
#define ZMDNET_ABORT            0x0200 /* Send an ABORT to peer */
#define ZMDNET_UNORDERED        0x0400 /* Message is un-ordered */
#define ZMDNET_ADDR_OVER        0x0800 /* Override the primary-address */
#define ZMDNET_SENDALL          0x1000 /* Send this on all associations */
#define ZMDNET_EOR              0x2000 /* end of message signal */
#define ZMDNET_SACK_IMMEDIATELY 0x4000 /* Set I-Bit */

#define INVALID_SINFO_FLAG(x) (((x) & 0xfffffff0 \
                                    & ~(ZMDNET_EOF | ZMDNET_ABORT | ZMDNET_UNORDERED |\
				        ZMDNET_ADDR_OVER | ZMDNET_SENDALL | ZMDNET_EOR |\
					ZMDNET_SACK_IMMEDIATELY)) != 0)
/* for the endpoint */

/* The lower byte is an enumeration of PR-ZMDNET policies */
#define ZMDNET_PR_ZMDNET_NONE 0x0000 /* Reliable transfer */
#define ZMDNET_PR_ZMDNET_TTL  0x0001 /* Time based PR-ZMDNET */
#define ZMDNET_PR_ZMDNET_BUF  0x0002 /* Buffer based PR-ZMDNET */
#define ZMDNET_PR_ZMDNET_RTX  0x0003 /* Number of retransmissions based PR-ZMDNET */

#define PR_ZMDNET_POLICY(x)         ((x) & 0x0f)
#define PR_ZMDNET_ENABLED(x)        (PR_ZMDNET_POLICY(x) != ZMDNET_PR_ZMDNET_NONE)
#define PR_ZMDNET_TTL_ENABLED(x)    (PR_ZMDNET_POLICY(x) == ZMDNET_PR_ZMDNET_TTL)
#define PR_ZMDNET_BUF_ENABLED(x)    (PR_ZMDNET_POLICY(x) == ZMDNET_PR_ZMDNET_BUF)
#define PR_ZMDNET_RTX_ENABLED(x)    (PR_ZMDNET_POLICY(x) == ZMDNET_PR_ZMDNET_RTX)
#define PR_ZMDNET_INVALID_POLICY(x) (PR_ZMDNET_POLICY(x) > ZMDNET_PR_ZMDNET_RTX)

/*
 * user socket options: socket API defined
 */
/*
 * read-write options
 */
#define ZMDNET_RTOINFO                    0x00000001
#define ZMDNET_ASSOCINFO                  0x00000002
#define ZMDNET_INITMSG                    0x00000003
#define ZMDNET_NODELAY                    0x00000004
#define ZMDNET_AUTOCLOSE                  0x00000005
#define ZMDNET_PRIMARY_ADDR               0x00000007
#define ZMDNET_ADAPTATION_LAYER           0x00000008
#define ZMDNET_DISABLE_FRAGMENTS          0x00000009
#define ZMDNET_PEER_ADDR_PARAMS           0x0000000a
/* ancillary data/notification interest options */
/* Without this applied we will give V4 and V6 addresses on a V6 socket */
#define ZMDNET_I_WANT_MAPPED_V4_ADDR      0x0000000d
#define ZMDNET_MAXSEG                     0x0000000e
#define ZMDNET_DELAYED_SACK               0x0000000f
#define ZMDNET_FRAGMENT_INTERLEAVE        0x00000010
#define ZMDNET_PARTIAL_DELIVERY_POINT     0x00000011
/* authentication support */
#define ZMDNET_HMAC_IDENT                 0x00000014
#define ZMDNET_AUTH_ACTIVE_KEY            0x00000015
#define ZMDNET_AUTO_ASCONF                0x00000018
#define ZMDNET_MAX_BURST                  0x00000019
/* assoc level context */
#define ZMDNET_CONTEXT                    0x0000001a
/* explicit EOR signalling */
#define ZMDNET_EXPLICIT_EOR               0x0000001b
#define ZMDNET_REUSE_PORT                 0x0000001c

#define ZMDNET_EVENT                      0x0000001e
#define ZMDNET_RECVRCVINFO                0x0000001f
#define ZMDNET_RECVNXTINFO                0x00000020
#define ZMDNET_DEFAULT_SNDINFO            0x00000021
#define ZMDNET_DEFAULT_PRINFO             0x00000022
#define ZMDNET_REMOTE_UDP_ENCAPS_PORT     0x00000024

#define ZMDNET_ENABLE_STREAM_RESET        0x00000900 /* struct zmdnet_assoc_value */

/* Pluggable Stream Scheduling Socket option */
#define ZMDNET_PLUGGABLE_SS               0x00001203
#define ZMDNET_SS_VALUE                   0x00001204

/*
 * read-only options
 */
#define ZMDNET_STATUS                     0x00000100
#define ZMDNET_GET_PEER_ADDR_INFO         0x00000101
/* authentication support */
#define ZMDNET_PEER_AUTH_CHUNKS           0x00000102
#define ZMDNET_LOCAL_AUTH_CHUNKS          0x00000103
#define ZMDNET_GET_ASSOC_NUMBER           0x00000104
#define ZMDNET_GET_ASSOC_ID_LIST          0x00000105
#define ZMDNET_TIMEOUTS                   0x00000106
#define ZMDNET_PR_STREAM_STATUS           0x00000107
#define ZMDNET_PR_ASSOC_STATUS            0x00000108

/*
 * write-only options
 */
#define ZMDNET_SET_PEER_PRIMARY_ADDR      0x00000006
#define ZMDNET_AUTH_CHUNK                 0x00000012
#define ZMDNET_AUTH_KEY                   0x00000013
#define ZMDNET_AUTH_DEACTIVATE_KEY        0x0000001d
#define ZMDNET_AUTH_DELETE_KEY            0x00000016
#define ZMDNET_RESET_STREAMS              0x00000901 /* struct zmdnet_reset_streams */
#define ZMDNET_RESET_ASSOC                0x00000902 /* zmdnet_assoc_t */
#define ZMDNET_ADD_STREAMS                0x00000903 /* struct zmdnet_add_streams */

struct zmdnet_initmsg
{
  uint16_t sinit_num_ostreams;
  uint16_t sinit_max_instreams;
  uint16_t sinit_max_attempts;
  uint16_t sinit_max_init_timeo;
};

struct zmdnet_rtoinfo
{
  zmdnet_assoc_t srto_assoc_id;
  uint32_t srto_initial;
  uint32_t srto_max;
  uint32_t srto_min;
};

struct zmdnet_assocparams
{
  zmdnet_assoc_t sasoc_assoc_id;
  uint32_t sasoc_peer_rwnd;
  uint32_t sasoc_local_rwnd;
  uint32_t sasoc_cookie_life;
  uint16_t sasoc_asocmaxrxt;
  uint16_t sasoc_number_peer_destinations;
};

struct zmdnet_setprim
{
  struct sockaddr_storage ssp_addr;
  zmdnet_assoc_t ssp_assoc_id;
  uint8_t ssp_padding[4];
};

struct zmdnet_setadaptation
{
  uint32_t ssb_adaptation_ind;
};

struct zmdnet_paddrparams
{
  struct sockaddr_storage spp_address;
  zmdnet_assoc_t spp_assoc_id;
  uint32_t spp_hbinterval;
  uint32_t spp_pathmtu;
  uint32_t spp_flags;
  uint32_t spp_ipv6_flowlabel;
  uint16_t spp_pathmaxrxt;
  uint8_t spp_dscp;
};

#define SPP_HB_ENABLE       0x00000001
#define SPP_HB_DISABLE      0x00000002
#define SPP_HB_DEMAND       0x00000004
#define SPP_PMTUD_ENABLE    0x00000008
#define SPP_PMTUD_DISABLE   0x00000010
#define SPP_HB_TIME_IS_ZERO 0x00000080
#define SPP_IPV6_FLOWLABEL  0x00000100
#define SPP_DSCP            0x00000200

/* Used for ZMDNET_MAXSEG, ZMDNET_MAX_BURST, ZMDNET_ENABLE_STREAM_RESET, and ZMDNET_CONTEXT */
struct zmdnet_assoc_value
{
  zmdnet_assoc_t assoc_id;
  uint32_t assoc_value;
};

/* To enable stream reset */
#define ZMDNET_ENABLE_RESET_STREAM_REQ  0x00000001
#define ZMDNET_ENABLE_RESET_ASSOC_REQ   0x00000002
#define ZMDNET_ENABLE_CHANGE_ASSOC_REQ  0x00000004
#define ZMDNET_ENABLE_VALUE_MASK        0x00000007

struct zmdnet_reset_streams
{
  zmdnet_assoc_t srs_assoc_id;
  uint16_t srs_flags;
  uint16_t srs_number_streams; /* 0 == ALL */
  uint16_t srs_stream_list[]; /* list if strrst_num_streams is not 0 */
};

struct zmdnet_add_streams
{
  zmdnet_assoc_t sas_assoc_id;
  uint16_t sas_instrms;
  uint16_t sas_outstrms;
};

struct zmdnet_hmacalgo
{
  uint32_t shmac_number_of_idents;
  uint16_t shmac_idents[];
};

/* AUTH hmac_id */
#define ZMDNET_AUTH_HMAC_ID_RSVD    0x0000
#define ZMDNET_AUTH_HMAC_ID_SHA1    0x0001	/* default, mandatory */
#define ZMDNET_AUTH_HMAC_ID_SHA256  0x0003
#define ZMDNET_AUTH_HMAC_ID_SHA224  0x0004
#define ZMDNET_AUTH_HMAC_ID_SHA384  0x0005
#define ZMDNET_AUTH_HMAC_ID_SHA512  0x0006

struct zmdnet_sack_info
{
  zmdnet_assoc_t sack_assoc_id;
  uint32_t sack_delay;
  uint32_t sack_freq;
};

struct zmdnet_default_prinfo
{
  uint16_t pr_policy;
  uint32_t pr_value;
  zmdnet_assoc_t pr_assoc_id;
};

struct zmdnet_paddrinfo
{
  struct sockaddr_storage spinfo_address;
  zmdnet_assoc_t spinfo_assoc_id;
  int32_t spinfo_state;
  uint32_t spinfo_cwnd;
  uint32_t spinfo_srtt;
  uint32_t spinfo_rto;
  uint32_t spinfo_mtu;
};

struct zmdnet_status
{
  zmdnet_assoc_t sstat_assoc_id;
  int32_t sstat_state;
  uint32_t sstat_rwnd;
  uint16_t sstat_unackdata;
  uint16_t sstat_penddata;
  uint16_t sstat_instrms;
  uint16_t sstat_outstrms;
  uint32_t sstat_fragmentation_point;
  struct zmdnet_paddrinfo sstat_primary;
};

/*
 * user state values
 */
#define ZMDNET_CLOSED             0x0000
#define ZMDNET_BOUND              0x1000
#define ZMDNET_LISTEN             0x2000
#define ZMDNET_COOKIE_WAIT        0x0002
#define ZMDNET_COOKIE_ECHOED      0x0004
#define ZMDNET_ESTABLISHED        0x0008
#define ZMDNET_SHUTDOWN_SENT      0x0010
#define ZMDNET_SHUTDOWN_RECEIVED  0x0020
#define ZMDNET_SHUTDOWN_ACK_SENT  0x0040
#define ZMDNET_SHUTDOWN_PENDING   0x0080

#define ZMDNET_ACTIVE       0x0001  /* ZMDNET_ADDR_REACHABLE */
#define ZMDNET_INACTIVE     0x0002  /* neither ZMDNET_ADDR_REACHABLE
    nor ZMDNET_ADDR_UNCONFIRMED */
#define ZMDNET_UNCONFIRMED  0x0200  /* ZMDNET_ADDR_UNCONFIRMED */

struct zmdnet_authchunks
{
  zmdnet_assoc_t gauth_assoc_id;
  /*	uint32_t gauth_number_of_chunks; not available */
  uint8_t gauth_chunks[];
};

struct zmdnet_assoc_ids
{
  uint32_t gaids_number_of_ids;
  zmdnet_assoc_t gaids_assoc_id[];
};

struct zmdnet_setpeerprim
{
  struct sockaddr_storage sspp_addr;
  zmdnet_assoc_t sspp_assoc_id;
  uint8_t sspp_padding[4];
};

struct zmdnet_authchunk
{
  uint8_t sauth_chunk;
};

struct zmdnet_get_nonce_values
{
  zmdnet_assoc_t gn_assoc_id;
  uint32_t gn_peers_tag;
  uint32_t gn_local_tag;
};

/*
 * Main ZMDNET chunk types
 */
/************0x00 series ***********/
#define ZMDNET_DATA               0x00
#define ZMDNET_INITIATION         0x01
#define ZMDNET_INITIATION_ACK     0x02
#define ZMDNET_SELECTIVE_ACK      0x03
#define ZMDNET_HEARTBEAT_REQUEST  0x04
#define ZMDNET_HEARTBEAT_ACK      0x05
#define ZMDNET_ABORT_ASSOCIATION  0x06
#define ZMDNET_SHUTDOWN           0x07
#define ZMDNET_SHUTDOWN_ACK       0x08
#define ZMDNET_OPERATION_ERROR    0x09
#define ZMDNET_COOKIE_ECHO        0x0a
#define ZMDNET_COOKIE_ACK         0x0b
#define ZMDNET_ECN_ECHO           0x0c
#define ZMDNET_ECN_CWR            0x0d
#define ZMDNET_SHUTDOWN_COMPLETE  0x0e
/* RFC4895 */
#define ZMDNET_AUTHENTICATION     0x0f
/* EY nr_sack chunk id*/
#define ZMDNET_NR_SELECTIVE_ACK   0x10
/************0x40 series ***********/
/************0x80 series ***********/
/* RFC5061 */
#define	ZMDNET_ASCONF_ACK         0x80
/* draft-ietf-stewart-pktdrpzmdnet */
#define ZMDNET_PACKET_DROPPED     0x81
/* draft-ietf-stewart-strreset-xxx */
#define ZMDNET_STREAM_RESET       0x82

/* RFC4820                         */
#define ZMDNET_PAD_CHUNK          0x84
/************0xc0 series ***********/
/* RFC3758 */
#define ZMDNET_FORWARD_CUM_TSN    0xc0
/* RFC5061 */
#define ZMDNET_ASCONF             0xc1

struct zmdnet_authkey
{
  zmdnet_assoc_t sca_assoc_id;
  uint16_t sca_keynumber;
  uint16_t sca_keylength;
  uint8_t sca_key[];
};

struct zmdnet_authkeyid
{
  zmdnet_assoc_t scact_assoc_id;
  uint16_t scact_keynumber;
};

struct zmdnet_cc_option
{
  int option;
  struct zmdnet_assoc_value aid_value;
};

struct zmdnet_stream_value
{
  zmdnet_assoc_t assoc_id;
  uint16_t stream_id;
  uint16_t stream_value;
};

struct zmdnet_timeouts
{
  zmdnet_assoc_t stimo_assoc_id;
  uint32_t stimo_init;
  uint32_t stimo_data;
  uint32_t stimo_sack;
  uint32_t stimo_shutdown;
  uint32_t stimo_heartbeat;
  uint32_t stimo_cookie;
  uint32_t stimo_shutdownack;
};

struct zmdnet_prstatus
{
  zmdnet_assoc_t sprstat_assoc_id;
  uint16_t sprstat_sid;
  uint16_t sprstat_policy;
  uint64_t sprstat_abandoned_unsent;
  uint64_t sprstat_abandoned_sent;
};

/* Standard TCP Congestion Control */
#define ZMDNET_CC_RFC2581         0x00000000
/* High Speed TCP Congestion Control (Floyd) */
#define ZMDNET_CC_HSTCP           0x00000001
/* HTCP Congestion Control */
#define ZMDNET_CC_HTCP            0x00000002
/* RTCC Congestion Control - RFC2581 plus */
#define ZMDNET_CC_RTCC            0x00000003

#define ZMDNET_CC_OPT_RTCC_SETMODE 0x00002000
#define ZMDNET_CC_OPT_USE_DCCC_EC  0x00002001
#define ZMDNET_CC_OPT_STEADY_STEP  0x00002002

#define ZMDNET_CMT_OFF            0
#define ZMDNET_CMT_BASE           1
#define ZMDNET_CMT_RPV1           2
#define ZMDNET_CMT_RPV2           3
#define ZMDNET_CMT_MPTCP          4
#define ZMDNET_CMT_MAX            ZMDNET_CMT_MPTCP

/* RS - Supported stream scheduling modules for pluggable
 * stream scheduling
 */
/* Default simple round-robin */
#define ZMDNET_SS_DEFAULT             0x00000000
/* Real round-robin */
#define ZMDNET_SS_ROUND_ROBIN         0x00000001
/* Real round-robin per packet */
#define ZMDNET_SS_ROUND_ROBIN_PACKET  0x00000002
/* Priority */
#define ZMDNET_SS_PRIORITY            0x00000003
/* Fair Bandwidth */
#define ZMDNET_SS_FAIR_BANDWITH       0x00000004
/* First-come, first-serve */
#define ZMDNET_SS_FIRST_COME          0x00000005

/******************** System calls *************/

struct socket;

void
zmdnet_init (uint16_t, int
(*) (void *addr, void *buffer, size_t length, uint8_t tos, uint8_t set_df),
	     void
	     (*) (const char *format, ...));

struct socket *
zmdnet_socket (
    int domain,
    int type,
    int protocol,
    int
    (*receive_cb) (struct socket *sock, union zmdnet_sockstore addr, void *data,
		   size_t datalen, struct zmdnet_rcvinfo, int flags,
		   void *ulp_info),
    int
    (*send_cb) (struct socket *sock, uint32_t sb_free),
    uint32_t sb_threshold, void *ulp_info);

int
zmdnet_setsockopt (struct socket *so, int level, int option_name,
		   const void *option_value, socklen_t option_len);

int
zmdnet_getsockopt (struct socket *so, int level, int option_name,
		   void *option_value, socklen_t *option_len);

int
zmdnet_opt_info (struct socket *so, zmdnet_assoc_t id, int opt, void *arg,
		 socklen_t *size);

int
zmdnet_getpaddrs (struct socket *so, zmdnet_assoc_t id,
		  struct sockaddr **raddrs);

void
zmdnet_freepaddrs (struct sockaddr *addrs);

int
zmdnet_getladdrs (struct socket *so, zmdnet_assoc_t id,
		  struct sockaddr **raddrs);

void
zmdnet_freeladdrs (struct sockaddr *addrs);

ssize_t
zmdnet_sendv (struct socket *so, const void *data, size_t len,
	      struct sockaddr *to, int addrcnt, void *info, socklen_t infolen,
	      unsigned int infotype, int flags);

ssize_t
zmdnet_recvv (struct socket *so, void *dbuf, size_t len, struct sockaddr *from,
	      socklen_t * fromlen, void *info, socklen_t *infolen,
	      unsigned int *infotype, int *msg_flags);

int
zmdnet_bind (struct socket *so, struct sockaddr *name, socklen_t namelen);

#define ZMDNET_BINDX_ADD_ADDR 0x00008001
#define ZMDNET_BINDX_REM_ADDR 0x00008002

int
zmdnet_bindx (struct socket *so, struct sockaddr *addrs, int addrcnt, int flags);

int
zmdnet_listen (struct socket *so, int backlog);

struct socket *
zmdnet_accept (struct socket *so, struct sockaddr * aname, socklen_t * anamelen);

struct socket *
zmdnet_peeloff (struct socket *, zmdnet_assoc_t);

int
zmdnet_connect (struct socket *so, struct sockaddr *name, socklen_t namelen);

int
zmdnet_connectx (struct socket *so, const struct sockaddr *addrs, int addrcnt,
		 zmdnet_assoc_t *id);

void
zmdnet_close (struct socket *so);

zmdnet_assoc_t
zmdnet_getassocid (struct socket *, struct sockaddr *);

int
zmdnet_finish (void);

int
zmdnet_shutdown (struct socket *so, int how);

void
zmdnet_conninput (void *, const void *, size_t, uint8_t);

int
zmdnet_set_non_blocking (struct socket *, int);

int
zmdnet_get_non_blocking (struct socket *);

void
zmdnet_register_address (void *);

void
zmdnet_deregister_address (void *);

int
zmdnet_set_ulpinfo (struct socket *, void *);

#define ZMDNET_DUMP_OUTBOUND 1
#define ZMDNET_DUMP_INBOUND  0

char *
zmdnet_dumppacket (const void *, size_t, int);

void
zmdnet_freedumpbuffer (char *);

void
zmdnet_enable_crc32c_offload (void);

void
zmdnet_disable_crc32c_offload (void);

uint32_t
zmdnet_crc32c (void *, size_t);

#define ZMDNET_SYSCTL_DECL(__field)                \
void zmdnet_sysctl_set_ ## __field(uint32_t value);\
uint32_t zmdnet_sysctl_get_ ## __field(void);

ZMDNET_SYSCTL_DECL(zmdnet_sendspace)
ZMDNET_SYSCTL_DECL(zmdnet_recvspace)
ZMDNET_SYSCTL_DECL(zmdnet_auto_asconf)
ZMDNET_SYSCTL_DECL(zmdnet_multiple_asconfs)
ZMDNET_SYSCTL_DECL(zmdnet_ecn_enable)
ZMDNET_SYSCTL_DECL(zmdnet_pr_enable)
ZMDNET_SYSCTL_DECL(zmdnet_auth_enable)
ZMDNET_SYSCTL_DECL(zmdnet_asconf_enable)
ZMDNET_SYSCTL_DECL(zmdnet_reconfig_enable)
ZMDNET_SYSCTL_DECL(zmdnet_nrsack_enable)
ZMDNET_SYSCTL_DECL(zmdnet_pktdrop_enable)
#if !defined(ZMDNET_WITH_NO_CSUM)
ZMDNET_SYSCTL_DECL(zmdnet_no_csum_on_loopback)
#endif
ZMDNET_SYSCTL_DECL(zmdnet_peer_chunk_oh)
ZMDNET_SYSCTL_DECL(zmdnet_max_burst_default)
ZMDNET_SYSCTL_DECL(zmdnet_max_chunks_on_queue)
ZMDNET_SYSCTL_DECL(zmdnet_hashtblsize)
ZMDNET_SYSCTL_DECL(zmdnet_pcbtblsize)
ZMDNET_SYSCTL_DECL(zmdnet_min_split_point)
ZMDNET_SYSCTL_DECL(zmdnet_chunkscale)
ZMDNET_SYSCTL_DECL(zmdnet_delayed_sack_time_default)
ZMDNET_SYSCTL_DECL(zmdnet_sack_freq_default)
ZMDNET_SYSCTL_DECL(zmdnet_system_free_resc_limit)
ZMDNET_SYSCTL_DECL(zmdnet_asoc_free_resc_limit)
ZMDNET_SYSCTL_DECL(zmdnet_heartbeat_interval_default)
ZMDNET_SYSCTL_DECL(zmdnet_pmtu_raise_time_default)
ZMDNET_SYSCTL_DECL(zmdnet_shutdown_guard_time_default)
ZMDNET_SYSCTL_DECL(zmdnet_secret_lifetime_default)
ZMDNET_SYSCTL_DECL(zmdnet_rto_max_default)
ZMDNET_SYSCTL_DECL(zmdnet_rto_min_default)
ZMDNET_SYSCTL_DECL(zmdnet_rto_initial_default)
ZMDNET_SYSCTL_DECL(zmdnet_init_rto_max_default)
ZMDNET_SYSCTL_DECL(zmdnet_valid_cookie_life_default)
ZMDNET_SYSCTL_DECL(zmdnet_init_rtx_max_default)
ZMDNET_SYSCTL_DECL(zmdnet_assoc_rtx_max_default)
ZMDNET_SYSCTL_DECL(zmdnet_path_rtx_max_default)
ZMDNET_SYSCTL_DECL(zmdnet_add_more_threshold)
ZMDNET_SYSCTL_DECL(zmdnet_nr_incoming_streams_default)
ZMDNET_SYSCTL_DECL(zmdnet_nr_outgoing_streams_default)
ZMDNET_SYSCTL_DECL(zmdnet_cmt_on_off)
ZMDNET_SYSCTL_DECL(zmdnet_cmt_use_dac)
ZMDNET_SYSCTL_DECL(zmdnet_use_cwnd_based_maxburst)
ZMDNET_SYSCTL_DECL(zmdnet_nat_friendly)
ZMDNET_SYSCTL_DECL(zmdnet_L2_abc_variable)
ZMDNET_SYSCTL_DECL(zmdnet_mbuf_threshold_count)
ZMDNET_SYSCTL_DECL(zmdnet_do_drain)
ZMDNET_SYSCTL_DECL(zmdnet_hb_maxburst)
ZMDNET_SYSCTL_DECL(zmdnet_abort_if_one_2_one_hits_limit)
ZMDNET_SYSCTL_DECL(zmdnet_min_residual)
ZMDNET_SYSCTL_DECL(zmdnet_max_retran_chunk)
ZMDNET_SYSCTL_DECL(zmdnet_logging_level)
ZMDNET_SYSCTL_DECL(zmdnet_default_cc_module)
ZMDNET_SYSCTL_DECL(zmdnet_default_frag_interleave)
ZMDNET_SYSCTL_DECL(zmdnet_mobility_base)
ZMDNET_SYSCTL_DECL(zmdnet_mobility_fasthandoff)
ZMDNET_SYSCTL_DECL(zmdnet_inits_include_nat_friendly)
ZMDNET_SYSCTL_DECL(zmdnet_udp_tunneling_port)
ZMDNET_SYSCTL_DECL(zmdnet_enable_sack_immediately)
ZMDNET_SYSCTL_DECL(zmdnet_vtag_time_wait)
ZMDNET_SYSCTL_DECL(zmdnet_blackhole)
ZMDNET_SYSCTL_DECL(zmdnet_diag_info_code)
ZMDNET_SYSCTL_DECL(zmdnet_fr_max_burst_default)
ZMDNET_SYSCTL_DECL(zmdnet_path_pf_threshold)
ZMDNET_SYSCTL_DECL(zmdnet_default_ss_module)
ZMDNET_SYSCTL_DECL(zmdnet_rttvar_bw)
ZMDNET_SYSCTL_DECL(zmdnet_rttvar_rtt)
ZMDNET_SYSCTL_DECL(zmdnet_rttvar_eqret)
ZMDNET_SYSCTL_DECL(zmdnet_steady_step)
ZMDNET_SYSCTL_DECL(zmdnet_use_dccc_ecn)
ZMDNET_SYSCTL_DECL(zmdnet_buffer_splitting)
ZMDNET_SYSCTL_DECL(zmdnet_initial_cwnd)
#ifdef ZMDNET_DEBUG
ZMDNET_SYSCTL_DECL(zmdnet_debug_on)
/* More specific values can be found in zmdnet_constants, but
 * are not considered to be part of the API.
 */
#define ZMDNET_DEBUG_NONE 0x00000000
#define ZMDNET_DEBUG_ALL  0xffffffff
#endif
#undef ZMDNET_SYSCTL_DECL

struct zmdnet_timeval
{
  uint32_t tv_sec;
  uint32_t tv_usec;
};

struct zmdnetstat {
	struct zmdnet_timeval zmdnets_discontinuitytime; /* zmdnetStats 18 (TimeStamp) */
	/* MIB according to RFC 3873 */
	uint32_t  zmdnets_currestab;           /* zmdnetStats  1   (Gauge32) */
	uint32_t  zmdnets_activeestab;         /* zmdnetStats  2 (Counter32) */
	uint32_t  zmdnets_restartestab;
	uint32_t  zmdnets_collisionestab;
	uint32_t  zmdnets_passiveestab;        /* zmdnetStats  3 (Counter32) */
	uint32_t  zmdnets_aborted;             /* zmdnetStats  4 (Counter32) */
	uint32_t  zmdnets_shutdown;            /* zmdnetStats  5 (Counter32) */
	uint32_t  zmdnets_outoftheblue;        /* zmdnetStats  6 (Counter32) */
	uint32_t  zmdnets_checksumerrors;      /* zmdnetStats  7 (Counter32) */
	uint32_t  zmdnets_outcontrolchunks;    /* zmdnetStats  8 (Counter64) */
	uint32_t  zmdnets_outorderchunks;      /* zmdnetStats  9 (Counter64) */
	uint32_t  zmdnets_outunorderchunks;    /* zmdnetStats 10 (Counter64) */
	uint32_t  zmdnets_incontrolchunks;     /* zmdnetStats 11 (Counter64) */
	uint32_t  zmdnets_inorderchunks;       /* zmdnetStats 12 (Counter64) */
	uint32_t  zmdnets_inunorderchunks;     /* zmdnetStats 13 (Counter64) */
	uint32_t  zmdnets_fragusrmsgs;         /* zmdnetStats 14 (Counter64) */
	uint32_t  zmdnets_reasmusrmsgs;        /* zmdnetStats 15 (Counter64) */
	uint32_t  zmdnets_outpackets;          /* zmdnetStats 16 (Counter64) */
	uint32_t  zmdnets_inpackets;           /* zmdnetStats 17 (Counter64) */

	/* input statistics: */
	uint32_t  zmdnets_recvpackets;         /* total input packets        */
	uint32_t  zmdnets_recvdatagrams;       /* total input datagrams      */
	uint32_t  zmdnets_recvpktwithdata;     /* total packets that had data */
	uint32_t  zmdnets_recvsacks;           /* total input SACK chunks    */
	uint32_t  zmdnets_recvdata;            /* total input DATA chunks    */
	uint32_t  zmdnets_recvdupdata;         /* total input duplicate DATA chunks */
	uint32_t  zmdnets_recvheartbeat;       /* total input HB chunks      */
	uint32_t  zmdnets_recvheartbeatack;    /* total input HB-ACK chunks  */
	uint32_t  zmdnets_recvecne;            /* total input ECNE chunks    */
	uint32_t  zmdnets_recvauth;            /* total input AUTH chunks    */
	uint32_t  zmdnets_recvauthmissing;     /* total input chunks missing AUTH */
	uint32_t  zmdnets_recvivalhmacid;      /* total number of invalid HMAC ids received */
	uint32_t  zmdnets_recvivalkeyid;       /* total number of invalid secret ids received */
	uint32_t  zmdnets_recvauthfailed;      /* total number of auth failed */
	uint32_t  zmdnets_recvexpress;         /* total fast path receives all one chunk */
	uint32_t  zmdnets_recvexpressm;        /* total fast path multi-part data */
	uint32_t  zmdnets_recvnocrc;
	uint32_t  zmdnets_recvswcrc;
	uint32_t  zmdnets_recvhwcrc;

	/* output statistics: */
	uint32_t  zmdnets_sendpackets;         /* total output packets       */
	uint32_t  zmdnets_sendsacks;           /* total output SACKs         */
	uint32_t  zmdnets_senddata;            /* total output DATA chunks   */
	uint32_t  zmdnets_sendretransdata;     /* total output retransmitted DATA chunks */
	uint32_t  zmdnets_sendfastretrans;     /* total output fast retransmitted DATA chunks */
	uint32_t  zmdnets_sendmultfastretrans; /* total FR's that happened more than once
	                                      * to same chunk (u-del multi-fr algo).
	                                      */
	uint32_t  zmdnets_sendheartbeat;       /* total output HB chunks     */
	uint32_t  zmdnets_sendecne;            /* total output ECNE chunks    */
	uint32_t  zmdnets_sendauth;            /* total output AUTH chunks FIXME   */
	uint32_t  zmdnets_senderrors;          /* ip_output error counter */
	uint32_t  zmdnets_sendnocrc;
	uint32_t  zmdnets_sendswcrc;
	uint32_t  zmdnets_sendhwcrc;
	/* PCKDROPREP statistics: */
	uint32_t  zmdnets_pdrpfmbox;           /* Packet drop from middle box */
	uint32_t  zmdnets_pdrpfehos;           /* P-drop from end host */
	uint32_t  zmdnets_pdrpmbda;            /* P-drops with data */
	uint32_t  zmdnets_pdrpmbct;            /* P-drops, non-data, non-endhost */
	uint32_t  zmdnets_pdrpbwrpt;           /* P-drop, non-endhost, bandwidth rep only */
	uint32_t  zmdnets_pdrpcrupt;           /* P-drop, not enough for chunk header */
	uint32_t  zmdnets_pdrpnedat;           /* P-drop, not enough data to confirm */
	uint32_t  zmdnets_pdrppdbrk;           /* P-drop, where process_chunk_drop said break */
	uint32_t  zmdnets_pdrptsnnf;           /* P-drop, could not find TSN */
	uint32_t  zmdnets_pdrpdnfnd;           /* P-drop, attempt reverse TSN lookup */
	uint32_t  zmdnets_pdrpdiwnp;           /* P-drop, e-host confirms zero-rwnd */
	uint32_t  zmdnets_pdrpdizrw;           /* P-drop, midbox confirms no space */
	uint32_t  zmdnets_pdrpbadd;            /* P-drop, data did not match TSN */
	uint32_t  zmdnets_pdrpmark;            /* P-drop, TSN's marked for Fast Retran */
	/* timeouts */
	uint32_t  zmdnets_timoiterator;        /* Number of iterator timers that fired */
	uint32_t  zmdnets_timodata;            /* Number of T3 data time outs */
	uint32_t  zmdnets_timowindowprobe;     /* Number of window probe (T3) timers that fired */
	uint32_t  zmdnets_timoinit;            /* Number of INIT timers that fired */
	uint32_t  zmdnets_timosack;            /* Number of sack timers that fired */
	uint32_t  zmdnets_timoshutdown;        /* Number of shutdown timers that fired */
	uint32_t  zmdnets_timoheartbeat;       /* Number of heartbeat timers that fired */
	uint32_t  zmdnets_timocookie;          /* Number of times a cookie timeout fired */
	uint32_t  zmdnets_timosecret;          /* Number of times an endpoint changed its cookie secret*/
	uint32_t  zmdnets_timopathmtu;         /* Number of PMTU timers that fired */
	uint32_t  zmdnets_timoshutdownack;     /* Number of shutdown ack timers that fired */
	uint32_t  zmdnets_timoshutdownguard;   /* Number of shutdown guard timers that fired */
	uint32_t  zmdnets_timostrmrst;         /* Number of stream reset timers that fired */
	uint32_t  zmdnets_timoearlyfr;         /* Number of early FR timers that fired */
	uint32_t  zmdnets_timoasconf;          /* Number of times an asconf timer fired */
	uint32_t  zmdnets_timodelprim;	     /* Number of times a prim_deleted timer fired */
	uint32_t  zmdnets_timoautoclose;       /* Number of times auto close timer fired */
	uint32_t  zmdnets_timoassockill;       /* Number of asoc free timers expired */
	uint32_t  zmdnets_timoinpkill;         /* Number of inp free timers expired */
	/* former early FR counters */
	uint32_t  zmdnets_spare[11];
	/* others */
	uint32_t  zmdnets_hdrops;              /* packet shorter than header */
	uint32_t  zmdnets_badsum;              /* checksum error             */
	uint32_t  zmdnets_noport;              /* no endpoint for port       */
	uint32_t  zmdnets_badvtag;             /* bad v-tag                  */
	uint32_t  zmdnets_badsid;              /* bad SID                    */
	uint32_t  zmdnets_nomem;               /* no memory                  */
	uint32_t  zmdnets_fastretransinrtt;    /* number of multiple FR in a RTT window */
	uint32_t  zmdnets_markedretrans;
	uint32_t  zmdnets_naglesent;           /* nagle allowed sending      */
	uint32_t  zmdnets_naglequeued;         /* nagle doesn't allow sending */
	uint32_t  zmdnets_maxburstqueued;      /* max burst doesn't allow sending */
	uint32_t  zmdnets_ifnomemqueued;       /* look ahead tells us no memory in
	                                      * interface ring buffer OR we had a
	                                      * send error and are queuing one send.
	                                      */
	uint32_t  zmdnets_windowprobed;        /* total number of window probes sent */
	uint32_t  zmdnets_lowlevelerr;         /* total times an output error causes us
	                                      * to clamp down on next user send.
	                                      */
	uint32_t  zmdnets_lowlevelerrusr;      /* total times zmdnet_senderrors were caused from
	                                      * a user send from a user invoked send not
	                                      * a sack response
	                                      */
	uint32_t  zmdnets_datadropchklmt;      /* Number of in data drops due to chunk limit reached */
	uint32_t  zmdnets_datadroprwnd;        /* Number of in data drops due to rwnd limit reached */
	uint32_t  zmdnets_ecnereducedcwnd;     /* Number of times a ECN reduced the cwnd */
	uint32_t  zmdnets_vtagexpress;         /* Used express lookup via vtag */
	uint32_t  zmdnets_vtagbogus;           /* Collision in express lookup. */
	uint32_t  zmdnets_primary_randry;      /* Number of times the sender ran dry of user data on primary */
	uint32_t  zmdnets_cmt_randry;          /* Same for above */
	uint32_t  zmdnets_slowpath_sack;       /* Sacks the slow way */
	uint32_t  zmdnets_wu_sacks_sent;       /* Window Update only sacks sent */
	uint32_t  zmdnets_sends_with_flags;    /* number of sends with sinfo_flags !=0 */
	uint32_t  zmdnets_sends_with_unord;    /* number of unordered sends */
	uint32_t  zmdnets_sends_with_eof;      /* number of sends with EOF flag set */
	uint32_t  zmdnets_sends_with_abort;    /* number of sends with ABORT flag set */
	uint32_t  zmdnets_protocol_drain_calls;/* number of times protocol drain called */
	uint32_t  zmdnets_protocol_drains_done;/* number of times we did a protocol drain */
	uint32_t  zmdnets_read_peeks;          /* Number of times recv was called with peek */
	uint32_t  zmdnets_cached_chk;          /* Number of cached chunks used */
	uint32_t  zmdnets_cached_strmoq;       /* Number of cached stream oq's used */
	uint32_t  zmdnets_left_abandon;        /* Number of unread messages abandoned by close */
	uint32_t  zmdnets_send_burst_avoid;    /* Unused */
	uint32_t  zmdnets_send_cwnd_avoid;     /* Send cwnd full  avoidance, already max burst inflight to net */
	uint32_t  zmdnets_fwdtsn_map_over;     /* number of map array over-runs via fwd-tsn's */
	uint32_t  zmdnets_queue_upd_ecne;      /* Number of times we queued or updated an ECN chunk on send queue */
	uint32_t  zmdnets_reserved[31];        /* Future ABI compat - remove int's from here when adding new */
};
void
zmdnet_get_stat (struct zmdnetstat *);

#ifdef _WIN32
#ifdef _MSC_VER
#pragma warning(default: 4200)
#endif
#endif

#ifdef  __cplusplus
}
#endif

#endif
