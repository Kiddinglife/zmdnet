/*-
 * Copyright (c) 1982, 1986, 1990, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef _USER_SOCKETVAR_H_
#define _USER_SOCKETVAR_H_

#include "zmdnet-queue.h"

#if defined(__DragonFly__)
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(__DragonFly__) || defined(__FreeBSD__) || defined(_WIN32) || defined(__NetBSD__) || defined(__native_client__)
#include <sys/uio.h>
#endif

#if defined(__APPLE__) || defined(__DragonFly__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (_WIN32) || defined(__native_client__)
#define UIO_MAXIOV 1024
#define ERESTART (-1)
#endif

#if !defined(__APPLE__) && !defined(__OpenBSD__) && !defined (__OpenBSD__) && !defined(__native_client__)
enum uio_rw
{
  UIO_READ, UIO_WRITE
};
#endif

#ifndef MSG_NOTIFICATION
#define MSG_NOTIFICATION 0x2000         /* zmdnet notification */
#endif
#define SCTP_SO_LINGER     0x0001
#define SCTP_SO_ACCEPTCONN 0x0002
#define SS_CANTRCVMORE 0x020
#define SS_CANTSENDMORE 0x010

#if !defined(__NetBSD__) && !defined(__OpenBSD__)
/* Segment flag values. */
enum uio_seg
{
  UIO_USERSPACE, /* from user data space */
  UIO_SYSSPACE /* from system space */
};
#endif

/* struct proc is a dummy for user space*/
struct proc
{
  int stub;
};

struct uio
{
  struct iovec *uio_iov; /* scatter/gather list */
  int uio_iovcnt; /* length of scatter/gather list */
  off_t uio_offset; /* offset in target object */
  ssize_t uio_resid; /* remaining bytes to process */
  enum uio_seg uio_segflg; /* address space */
  enum uio_rw uio_rw; /* operation */
};

/*
 * Kernel structure per socket.
 * Contains send and receive buffer queues,
 * handle on protocol and pointer to protocol
 * private data and error information.
 */
#if defined (_WIN32)
#define AF_ROUTE  17
typedef __int32 pid_t;
typedef unsigned __int32 uid_t;
enum sigType
{
  SIGNAL = 0,
  BROADCAST = 1,
  MAX_EVENTS = 2
};
#endif

/*
 * Constants for sb_flags field of struct sockbuf.
 */
#define SB_MAX      (256*1024)  /* default for max chars in sockbuf */
#define SB_RAW          (64*1024*2)    /*Aligning so->so_rcv.sb_hiwat with the receive buffer size of raw socket*/
/*
 * Constants for sb_flags field of struct sockbuf.
 */
#define SB_WAIT     0x04        /* someone is waiting for data/space */
#define SB_SEL      0x08        /* someone is selecting */
#define SB_ASYNC    0x10        /* ASYNC I/O, need signals */
#define SB_UPCALL   0x20        /* someone wants an upcall */
#define SB_NOINTR   0x40        /* operations not interruptible */
#define SB_AIO      0x80        /* AIO operations queued */
#define SB_KNOTE    0x100       /* kernel note attached */
#define SB_AUTOSIZE 0x800       /* automatically size socket buffer */

/*-
 * Locking key to struct socket:
 * (a) constant after allocation, no locking required.
 * (b) locked by SOCK_LOCK(so).
 * (c) locked by SOCKBUF_LOCK(&so->so_rcv).
 * (d) locked by SOCKBUF_LOCK(&so->so_snd).
 * (e) locked by ACCEPT_LOCK().
 * (f) not locked since integer reads/writes are atomic.
 * (g) used only as a sleep/wakeup address, no value.
 * (h) locked by global mutex so_global_mtx.
 */
struct socket
{
  int so_count; /* (b) reference count */
  short so_type; /* (a) generic type, see socket.h */
  short so_options; /* from socket call, see socket.h */
  short so_linger; /* time to linger while closing */
  short so_state; /* (b) internal state flags SS_* */
  int so_qstate; /* (e) internal state flags SQ_* */
  void *so_pcb; /* protocol control block */
  int so_dom;

  /*
   * Variables for connection queuing.
   * Socket where accepts occur is so_head in all subsidiary sockets.
   * If so_head is 0, socket is not related to an accept.
   * For head socket so_incomp queues partially completed connections,
   * while so_comp is a queue of connections ready to be accepted.
   * If a connection is aborted and it has so_head set, then
   * it has to be pulled out of either so_incomp or so_comp.
   * We allow connections to queue up based on current queue lengths
   * and limit on number of queued connections for this socket.
   */
  struct socket *so_head; /* (e) back pointer to listen socket */
  TAILQ_HEAD(, socket)
  so_incomp; /* (e) queue of partial unaccepted connections */
  TAILQ_HEAD(, socket)
  so_comp; /* (e) queue of complete unaccepted connections */
  TAILQ_ENTRY(socket)
  so_list; /* (e) list of unaccepted connections */
  u_short so_qlen; /* (e) number of unaccepted connections */
  u_short so_incqlen; /* (e) number of unaccepted incomplete
   connections */
  u_short so_qlimit; /* (e) max number queued connections */
  short so_timeo; /* (g) connection timeout */
  zmdnet_cond_t timeo_cond; /* timeo_cond condition variable being used in wakeup */

  u_short so_error; /* (f) error affecting connection */
  struct sigio *so_sigio; /* [sg] information for async I/O or
   out of band data (SIGURG) */
  u_long so_oobmark; /* (c) chars to oob mark */
  TAILQ_HEAD(, aiocblist)
  so_aiojobq; /* AIO ops waiting on socket */

  /*
   * Variables for socket buffering.
   */
  struct sockbuf
  {
    /* __Userspace__ Many of these fields may
     * not be required for the sctp stack.
     * Commenting out the following.
     * Including pthread mutex and condition variable to be
     * used by sbwait, sorwakeup and sowwakeup.
     */
    /* struct   selinfo sb_sel;*//* process selecting read/write */
    /* struct   mtx sb_mtx;*//* sockbuf lock */
    /* struct   sx sb_sx;*//* prevent I/O interlacing */
    zmdnet_cond_t sb_cond; /* sockbuf condition variable */
    zmdnet_mutex_t sb_mtx; /* sockbuf lock associated with sb_cond */
    short sb_state; /* (c/d) socket state on sockbuf */
#define sb_startzero    sb_mb
    struct mbuf *sb_mb; /* (c/d) the mbuf chain */
    struct mbuf *sb_mbtail; /* (c/d) the last mbuf in the chain */
    struct mbuf *sb_lastrecord; /* (c/d) first mbuf of last record in socket buffer */
    struct mbuf *sb_sndptr; /* (c/d) pointer into mbuf chain */
    u_int sb_sndptroff; /* (c/d) byte offset of ptr into chain */
    u_int sb_cc; /* (c/d) actual chars in buffer */
    u_int sb_hiwat; /* (c/d) max actual char count */
    u_int sb_mbcnt; /* (c/d) chars of mbufs used */
    u_int sb_mbmax; /* (c/d) max chars of mbufs to use */
    u_int sb_ctl; /* (c/d) non-data chars in buffer */
    int sb_lowat; /* (c/d) low water mark */
    int sb_timeo; /* (c/d) timeout for read/write */
    short sb_flags; /* (c/d) flags, see below */
  } so_rcv, so_snd;

  void (*so_upcall)(struct socket *, void *, int);
  void *so_upcallarg;
  struct ucred *so_cred; /* (a) user credentials */
  struct label *so_label; /* (b) MAC label for socket */
  struct label *so_peerlabel; /* (b) cached MAC label for peer */
  /* NB: generation count must not be first. */
  uint32_t so_gencnt; /* (h) generation count */
  void *so_emuldata; /* (b) private data for emulators */

  struct so_accf
  {
    struct accept_filter *so_accept_filter;
    void *so_accept_filter_arg; /* saved filter args */
    char *so_accept_filter_str; /* saved user args */
  }*so_accf;
};

#define SB_EMPTY_FIXUP(sb) do {                     \
    if ((sb)->sb_mb == NULL) {                  \
        (sb)->sb_mbtail = NULL;                 \
        (sb)->sb_lastrecord = NULL;             \
    }                               \
} while (/*CONSTCOND*/0)

#endif
