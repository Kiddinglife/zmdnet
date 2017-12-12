/*-
 * Copyright (c) 1982, 1986, 1988, 1993
 *      The Regents of the University of California.
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
 * 3. Neither the name of the University nor the names of its contributors
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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "zmdnet-mbuf.h"
#include "zmdnet-atomic.h"
//#include "netinet/zmdnet_pcb.h"

#define ZMDNET_ZONE_INIT(zone, name, size, number) {zone = size;}
#define ZMDNET_ZONE_GET(zone, type)  (type *)malloc(zone)
#define ZMDNET_ZONE_FREE(zone, element) {free(element);}
#define ZMDNET_ZONE_DESTROY(zone)

#ifndef ZMDNET_MALLOC_WAIT
#define ZMDNET_MALLOC_WAIT(mret,type,size) \
    do\
    {\
     mret = (type*) malloc(size);\
    } while (!mret)
#endif

#ifndef ZMDNET_DBG
#define ZMDNET_DBG(...)
#endif

#ifndef ZMDNET_DEBUG_USR
#define ZMDNET_DEBUG_USR 0x80000000
#endif

struct mbstat mbstat;
#define KIPC_MAX_LINKHDR        4       /* int: max length of link header (see sys/sysclt.h) */
#define KIPC_MAX_PROTOHDR   5   /* int: max length of network header (see sys/sysclt.h)*/
int max_linkhdr = KIPC_MAX_LINKHDR;
int max_protohdr = KIPC_MAX_PROTOHDR; /* Size of largest protocol layer header. */

/*
 * Zones from which we allocate.
 */
zmdnet_zone_t zone_mbuf;
zmdnet_zone_t zone_clust;
zmdnet_zone_t zone_ext_refcnt;

/* __Userspace__ clust_mb_args will be passed as callback data to mb_ctor_clust
 * and mb_dtor_clust.
 * Note: I had to use struct clust_args as an encapsulation for an mbuf pointer.
 * struct mbuf * clust_mb_args; does not work.
 */
struct clust_args clust_mb_args;

static int mb_ctor_mbuf(void *, void *, int);
static int mb_ctor_clust(void *, void *, int);
static void mb_dtor_mbuf(void*);
static void mb_dtor_clust(void *, void *);

/***************** Functions taken from user_mbuf.h *************/
struct mbuf* m_get(short type)
{
  struct mbuf *mret;
  struct mb_args mbuf_mb_args;
  mbuf_mb_args.flags = 0;
  mbuf_mb_args.type = type;
  // zone_mbuf has already been created in mbuf_initialize()
  ZMDNET_MALLOC_WAIT(mret, struct mbuf, zone_mbuf);
  mb_ctor_mbuf(mret, &mbuf_mb_args, 0);
  return mret;
}

struct mbuf* m_gethdr(short type)
{
  struct mbuf *mret;
  struct mb_args mbuf_mb_args;
  mbuf_mb_args.flags = M_PKTHDR;
  mbuf_mb_args.type = type;
  ZMDNET_MALLOC_WAIT(mret, struct mbuf, zone_mbuf);
  mb_ctor_mbuf(mret, &mbuf_mb_args, 0);
  return mret;
}

struct mbuf * m_free(struct mbuf *m)
{
  struct mbuf *n = m->m_next;
  if (m->m_flags & M_EXT)
    mb_free_ext(m);
  else if (!(m->m_flags & M_NOFREE))
  {
    mb_dtor_mbuf(m);
    free(m);
  }
  return (n);
}

static void clust_constructor(caddr_t m_clust, struct mbuf* m)
{
  u_int *refcnt;
  int type, size;

  if (m == NULL)
    return;

  /* Assigning cluster of MCLBYTES. TODO: Add jumbo frame functionality */
  type = EXT_CLUSTER;
  size = MCLBYTES;

  refcnt = ZMDNET_ZONE_GET(zone_ext_refcnt, u_int);
  *refcnt = 1;
  m->m_ext.ext_buf = (caddr_t) m_clust;
  m->m_data = m->m_ext.ext_buf;
  m->m_flags |= M_EXT;
  m->m_ext.ext_free = NULL;
  m->m_ext.ext_args = NULL;
  m->m_ext.ext_size = size;
  m->m_ext.ext_type = type;
  m->m_ext.ref_cnt = refcnt;
  return;
}

void m_clget(struct mbuf *m)
{
  caddr_t mclust_ret;
  struct clust_args clust_mb_args_l;
  if (m->m_flags & M_EXT)
  {
    ZMDNET_DBG(ZMDNET_DEBUG_USR, "%s: %p mbuf already has cluster\n", __func__,
        (void *) m);
  }
  m->m_ext.ext_buf = (char *) NULL;
  clust_mb_args_l.parent_mbuf = m;
  ZMDNET_MALLOC_WAIT(mclust_ret, char, zone_clust);
  mb_ctor_clust(mclust_ret, &clust_mb_args_l, 0);
  if ((m->m_ext.ext_buf == NULL))
  {
    clust_constructor(mclust_ret, m);
  }
}

/*
 * Unlink a tag from the list of tags associated with an mbuf.
 */
static __inline void m_tag_unlink(struct mbuf *m, struct m_tag *t)
{

  SLIST_REMOVE(&m->m_pkthdr.tags, t, m_tag, m_tag_link);
}

/*
 * Reclaim resources associated with a tag.
 */
static __inline void m_tag_free(struct m_tag *t)
{

  (*t->m_tag_free)(t);
}

/*
 * Set up the contents of a tag.  Note that this does not fill in the free
 * method; the caller is expected to do that.
 *
 * XXX probably should be called m_tag_init, but that was already taken.
 */
static __inline void m_tag_setup(struct m_tag *t, u_int32_t cookie, int type,
    int len)
{

  t->m_tag_id = type;
  t->m_tag_len = len;
  t->m_tag_cookie = cookie;
}

void mbuf_initialize(void *dummy)
{
  zone_mbuf = MSIZE;
  zone_ext_refcnt = sizeof(u_int);
  zone_clust = MCLBYTES;

  mbstat.m_mbufs = 0;
  mbstat.m_mclustsifnet = 0;
  mbstat.m_drain = 0;
  mbstat.m_msize = MSIZE;
  mbstat.m_mclbytes = MCLBYTES;
  mbstat.m_minclsize = MINCLSIZE;
  mbstat.m_mlen = MLEN;
  mbstat.m_mhlen = MHLEN;
  mbstat.m_numtypes = MT_NTYPES;

  mbstat.m_mcfail = mbstat.m_mpfail = 0;
  mbstat.sf_iocnt = 0;
  mbstat.sf_allocwait = mbstat.sf_allocfail = 0;
}

static int mb_ctor_mbuf(void *mem, void *arg, int flgs)
{
  struct mbuf *m;
  struct mb_args *args;

  int flags;
  short type;

  m = (struct mbuf *) mem;
  args = (struct mb_args *) arg;
  flags = args->flags;
  type = args->type;

  if (type == MT_NOINIT)
    return (0);

  m->m_next = NULL;
  m->m_nextpkt = NULL;
  m->m_len = 0;
  m->m_flags = flags;
  m->m_type = type;
  if (flags & M_PKTHDR)
  {
    m->m_data = m->m_pktdat;
    m->m_pkthdr.rcvif = NULL;
    m->m_pkthdr.len = 0;
    m->m_pkthdr.header = NULL;
    m->m_pkthdr.csum_flags = 0;
    m->m_pkthdr.csum_data = 0;
    m->m_pkthdr.tso_segsz = 0;
    m->m_pkthdr.ether_vtag = 0;
    SLIST_INIT(&m->m_pkthdr.tags);
  }
  else
    m->m_data = m->m_dat;
  return (0);
}

static void mb_dtor_mbuf(void *mem)
{
  struct mbuf *m = (struct mbuf *) mem;
  if (m->m_flags & M_PKTHDR)
  {
    m_tag_delete_chain(m, NULL);
  }
}

static int mb_ctor_clust(void *mem, void *arg, int flgs)
{
  struct mbuf *m;
  struct clust_args * cla;
  u_int *refcnt;
  int type, size;

  type = EXT_CLUSTER;
  size = MCLBYTES;

  cla = (struct clust_args *) arg;
  m = cla->parent_mbuf;

  refcnt = ZMDNET_ZONE_GET(zone_ext_refcnt, u_int);
  *refcnt = 1;

  if (m != NULL)
  {
    m->m_ext.ext_buf = (caddr_t) mem;
    m->m_data = m->m_ext.ext_buf;
    m->m_flags |= M_EXT;
    m->m_ext.ext_free = NULL;
    m->m_ext.ext_args = NULL;
    m->m_ext.ext_size = size;
    m->m_ext.ext_type = type;
    m->m_ext.ref_cnt = refcnt;
  }

  return (0);
}

static void mb_dtor_clust(void *mem, void *arg)
{

}

/* Unlink and free a packet tag. */
void m_tag_delete(struct mbuf *m, struct m_tag *t)
{
  ZMDNET_ASSERT(m && t,
      ("m_tag_delete: null argument, m %p t %p", (void *) m, (void *) t));
  m_tag_unlink(m, t);
  m_tag_free(t);
}

/* Unlink and free a packet tag chain, starting from given tag. */
void m_tag_delete_chain(struct mbuf *m, struct m_tag *t)
{

  struct m_tag *p, *q;

  ZMDNET_ASSERT(m, ("m_tag_delete_chain: null mbuf"));
  if (t != NULL)
    p = t;
  else
    p = SLIST_FIRST(&m->m_pkthdr.tags);
  if (p == NULL)
    return;
  while ((q = SLIST_NEXT(p, m_tag_link)) != NULL)
    m_tag_delete(m, q);
  m_tag_delete(m, p);
}

static void zmdnet_print_mbuf_chain(struct mbuf *m)
{
  ZMDNET_DBG(ZMDNET_DEBUG_USR, "Printing mbuf chain %p.\n", (void *) m);
  for (; m; m = m->m_next)
  {
    ZMDNET_DBG(ZMDNET_DEBUG_USR, "%p: m_len = %ld, m_type = %x, m_next = %p.\n", (void *) m, m->m_len, m->m_type, (void *) m->m_next);
    if (m->m_flags & M_EXT)
    {
      ZMDNET_DBG(ZMDNET_DEBUG_USR, "%p: extend_size = %d, extend_buffer = %p, ref_cnt = %d.\n", (void *) m, m->m_ext.ext_size, (void *) m->m_ext.ext_buf, *(m->m_ext.ref_cnt));
    }
  }
}

/*
 * Free an entire chain of mbufs and associated external buffers, if
 * applicable.
 */
void m_freem(struct mbuf *mb)
{
  while (mb != NULL)
    mb = m_free(mb);
}

/*
 * clean mbufs with M_EXT storage attached to them
 * if the reference count hits 1.
 */
void mb_free_ext(struct mbuf *m)
{

  int skipmbuf;

  ZMDNET_ASSERT((m->m_flags & M_EXT) == M_EXT, ("%s: M_EXT not set", __func__));
  ZMDNET_ASSERT(m->m_ext.ref_cnt != NULL, ("%s: ref_cnt not set", __func__));

  /*
   * check if the header is embedded in the cluster
   */
  skipmbuf = (m->m_flags & M_NOFREE);

#ifdef IPHONE
  if (atomic_fetchadd_int(m->m_ext.ref_cnt, -1) == 0)
#else
  if (ZMDNET_DECREMENT_AND_CHECK_REFCOUNT(m->m_ext.ref_cnt))
#endif
  {
    if (m->m_ext.ext_type == EXT_CLUSTER)
    {
      mb_dtor_clust(m->m_ext.ext_buf, &clust_mb_args);
      ZMDNET_ZONE_FREE(zone_clust, m->m_ext.ext_buf);
      ZMDNET_ZONE_FREE(zone_ext_refcnt, (u_int*) m->m_ext.ref_cnt);
      m->m_ext.ref_cnt = NULL;
    }
  }

  if (skipmbuf)
    return;

  /* __Userspace__ Also freeing the storage for ref_cnt
   * Free this mbuf back to the mbuf zone with all m_ext
   * information purged.
   */
  m->m_ext.ext_buf = NULL;
  m->m_ext.ext_free = NULL;
  m->m_ext.ext_args = NULL;
  m->m_ext.ref_cnt = NULL;
  m->m_ext.ext_size = 0;
  m->m_ext.ext_type = 0;
  m->m_flags &= ~M_EXT;
  mb_dtor_mbuf(m);
  ZMDNET_ZONE_FREE(zone_mbuf, m);

  /*umem_cache_free(zone_mbuf, m);*/
}

/*
 * "Move" mbuf pkthdr from "from" to "to".
 * "from" must have M_PKTHDR set, and "to" must be empty.
 */
void m_move_pkthdr(struct mbuf *to, struct mbuf *from)
{

  to->m_flags = (from->m_flags & M_COPYFLAGS) | (to->m_flags & M_EXT);
  if ((to->m_flags & M_EXT) == 0)
    to->m_data = to->m_pktdat;
  to->m_pkthdr = from->m_pkthdr; /* especially tags */
  SLIST_INIT(&from->m_pkthdr.tags); /* purge tags from src */
  from->m_flags &= ~M_PKTHDR;
}

/*
 * Rearange an mbuf chain so that len bytes are contiguous
 * and in the data area of an mbuf (so that mtod and dtom
 * will work for a structure of size len).  Returns the resulting
 * mbuf chain on success, frees it and returns null on failure.
 * If there is room, it will add up to max_protohdr-len extra bytes to the
 * contiguous region in an attempt to avoid being called next time.
 */
struct mbuf *
m_pullup(struct mbuf *n, int len)
{
  struct mbuf *m;
  int count;
  int space;

  /*
   * If first mbuf has no cluster, and has room for len bytes
   * without shifting current data, pullup into it,
   * otherwise allocate a new mbuf to prepend to the chain.
   */
  if ((n->m_flags & M_EXT) == 0&&
  n->m_data + len < &n->m_dat[MLEN] && n->m_next)
  {
    if (n->m_len >= len)
      return (n);
    m = n;
    n = n->m_next;
    len -= m->m_len;
  }
  else
  {
    if (len > MHLEN)
      goto bad;
    MGET(m, n->m_type);
    if (m == NULL)
      goto bad;
    m->m_len = 0;
    if (n->m_flags & M_PKTHDR)
      M_MOVE_PKTHDR(m, n);
  }
  space = (int) (&m->m_dat[MLEN] - (m->m_data + m->m_len));
  do
  {
    count = min(min(max(len, max_protohdr), space), n->m_len);
    memcpy(mtod(m, caddr_t) + m->m_len, mtod(n, caddr_t), (u_int) count);
    len -= count;
    m->m_len += count;
    n->m_len -= count;
    space -= count;
    if (n->m_len)
      n->m_data += count;
    else
      n = m_free(n);
  } while (len > 0 && n);
  if (len > 0)
  {
    (void) m_free(m);
    goto bad;
  }
  m->m_next = n;
  return (m);
  bad: m_freem(n);
  mbstat.m_mpfail++; /* XXX: No consistency. */
  return (NULL);
}

static struct mbuf *
m_dup1(struct mbuf *m, int off, int len)
{
  struct mbuf *n = NULL;
  int copyhdr;

  if (len > MCLBYTES)
    return NULL;
  if (off == 0 && (m->m_flags & M_PKTHDR) != 0)
    copyhdr = 1;
  else
    copyhdr = 0;
  if (len >= MINCLSIZE)
  {
    if (copyhdr == 1)
    {
      m_clget(n); /* TODO: include code for copying the header */
      m_dup_pkthdr(n, m);
    }
    else
      m_clget(n);
  }
  else
  {
    if (copyhdr == 1)
      n = m_gethdr(m->m_type);
    else
      n = m_get(m->m_type);
  }
  if (!n)
    return NULL; /* ENOBUFS */

  if (copyhdr && !m_dup_pkthdr(n, m))
  {
    m_free(n);
    return NULL;
  }
  m_copydata(m, off, len, mtod(n, caddr_t));
  n->m_len = len;
  return n;
}

/* Taken from sys/kern/uipc_mbuf2.c */
struct mbuf *
m_pulldown(struct mbuf *m, int off, int len, int *offp)
{
  struct mbuf *n, *o;
  int hlen, tlen, olen;
  int writable;

  /* check invalid arguments. */
  ZMDNET_ASSERT(m, ("m == NULL in m_pulldown()"));
  if (len > MCLBYTES)
  {
    m_freem(m);
    return NULL; /* impossible */
  }

#ifdef PULLDOWN_DEBUG
  {
    struct mbuf *t;
    ZMDNET_DEBUG_USR(ZMDNET_DEBUG_USR, "before:");
    for (t = m; t; t = t->m_next)
    ZMDNET_DEBUG_USR(ZMDNET_DEBUG_USR, " %d", t->m_len);
    ZMDNET_DEBUG_USR(ZMDNET_DEBUG_USR, "\n");
  }
#endif
  n = m;
  while (n != NULL && off > 0)
  {
    if (n->m_len > off)
      break;
    off -= n->m_len;
    n = n->m_next;
  }
  /* be sure to point non-empty mbuf */
  while (n != NULL && n->m_len == 0)
    n = n->m_next;
  if (!n)
  {
    m_freem(m);
    return NULL; /* mbuf chain too short */
  }

  writable = 0;
  if ((n->m_flags & M_EXT) == 0
      || (n->m_ext.ext_type == EXT_CLUSTER && M_WRITABLE(n)))
    writable = 1;

  /*
   * the target data is on <n, off>.
   * if we got enough data on the mbuf "n", we're done.
   */
  if ((off == 0 || offp) && len <= n->m_len - off && writable)
    goto ok;

  /*
   * when len <= n->m_len - off and off != 0, it is a special case.
   * len bytes from <n, off> sits in single mbuf, but the caller does
   * not like the starting position (off).
   * chop the current mbuf into two pieces, set off to 0.
   */
  if (len <= n->m_len - off)
  {
    o = m_dup1(n, off, n->m_len - off);
    if (o == NULL)
    {
      m_freem(m);
      return NULL; /* ENOBUFS */
    }
    n->m_len = off;
    o->m_next = n->m_next;
    n->m_next = o;
    n = n->m_next;
    off = 0;
    goto ok;
  }
  /*
   * we need to take hlen from <n, off> and tlen from <n->m_next, 0>,
   * and construct contiguous mbuf with m_len == len.
   * note that hlen + tlen == len, and tlen > 0.
   */
  hlen = n->m_len - off;
  tlen = len - hlen;

  /*
   * ensure that we have enough trailing data on mbuf chain.
   * if not, we can do nothing about the chain.
   */
  olen = 0;
  for (o = n->m_next; o != NULL; o = o->m_next)
    olen += o->m_len;
  if (hlen + olen < len)
  {
    m_freem(m);
    return NULL; /* mbuf chain too short */
  }

  /*
   * easy cases first.
   * we need to use m_copydata() to get data from <n->m_next, 0>.
   */
  if ((off == 0 || offp) && M_TRAILINGSPACE(n) >= tlen && writable)
  {
    m_copydata(n->m_next, 0, tlen, mtod(n, caddr_t) + n->m_len);
    n->m_len += tlen;
    m_adj(n->m_next, tlen);
    goto ok;
  }

  if ((off == 0 || offp) && M_LEADINGSPACE(n->m_next) >= hlen && writable)
  {
    n->m_next->m_data -= hlen;
    n->m_next->m_len += hlen;
    memcpy(mtod(n->m_next, caddr_t), mtod(n, caddr_t) + off, hlen);
    n->m_len -= hlen;
    n = n->m_next;
    off = 0;
    goto ok;
  }

  /*
   * now, we need to do the hard way.  don't m_copy as there's no room
   * on both end.
   */
  if (len > MLEN)
    m_clget(o);
  /* o = m_getcl(M_NOWAIT, m->m_type, 0);*/
  else
    o = m_get(m->m_type);
  if (!o)
  {
    m_freem(m);
    return NULL; /* ENOBUFS */
  }
  /* get hlen from <n, off> into <o, 0> */
  o->m_len = hlen;
  memcpy(mtod(o, caddr_t), mtod(n, caddr_t) + off, hlen);
  n->m_len -= hlen;
  /* get tlen from <n->m_next, 0> into <o, hlen> */
  m_copydata(n->m_next, 0, tlen, mtod(o, caddr_t) + o->m_len);
  o->m_len += tlen;
  m_adj(n->m_next, tlen);
  o->m_next = n->m_next;
  n->m_next = o;
  n = o;
  off = 0;
  ok:
#ifdef PULLDOWN_DEBUG
  {
    struct mbuf *t;
    ZMDNET_DEBUG_USR(ZMDNET_DEBUG_USR, "after:");
    for (t = m; t; t = t->m_next)
    ZMDNET_DEBUG_USR(ZMDNET_DEBUG_USR, "%c%d", t == n ? '*' : ' ', t->m_len);
    ZMDNET_DEBUG_USR(ZMDNET_DEBUG_USR, " (off=%d)\n", off);
  }
#endif
  if (offp)
    *offp = off;
  return n;
}

/*
 * Attach the the cluster from *m to *n, set up m_ext in *n
 * and bump the refcount of the cluster.
 */
static void mb_dupcl(struct mbuf *n, struct mbuf *m)
{
  ZMDNET_ASSERT((m->m_flags & M_EXT) == M_EXT, ("%s: M_EXT not set", __func__));
  ZMDNET_ASSERT(m->m_ext.ref_cnt != NULL, ("%s: ref_cnt not set", __func__));
  ZMDNET_ASSERT((n->m_flags & M_EXT) == 0, ("%s: M_EXT set", __func__));

  if (*(m->m_ext.ref_cnt) == 1)
    *(m->m_ext.ref_cnt) += 1;
  else
    atomic_add_int(m->m_ext.ref_cnt, 1);
  n->m_ext.ext_buf = m->m_ext.ext_buf;
  n->m_ext.ext_free = m->m_ext.ext_free;
  n->m_ext.ext_args = m->m_ext.ext_args;
  n->m_ext.ext_size = m->m_ext.ext_size;
  n->m_ext.ref_cnt = m->m_ext.ref_cnt;
  n->m_ext.ext_type = m->m_ext.ext_type;
  n->m_flags |= M_EXT;
}

/*
 * Make a copy of an mbuf chain starting "off0" bytes from the beginning,
 * continuing for "len" bytes.  If len is M_COPYALL, copy to end of mbuf.
 * The wait parameter is a choice of M_TRYWAIT/M_NOWAIT from caller.
 * Note that the copy is read-only, because clusters are not copied,
 * only their reference counts are incremented.
 */

struct mbuf *
m_copym(struct mbuf *m, int off0, int len, int wait)
{
  struct mbuf *n, **np;
  int off = off0;
  struct mbuf *top;
  int copyhdr = 0;

  ZMDNET_ASSERT(off >= 0, ("m_copym, negative off %d", off));
  ZMDNET_ASSERT(len >= 0, ("m_copym, negative len %d", len));

  if (off == 0 && (m->m_flags & M_PKTHDR))
    copyhdr = 1;
  while (off > 0)
  {
    ZMDNET_ASSERT(m != NULL, ("m_copym, offset > size of mbuf chain"));
    if (off < m->m_len)
      break;
    off -= m->m_len;
    m = m->m_next;
  }
  np = &top;
  top = 0;
  while (len > 0)
  {
    if (m == NULL)
    {
      ZMDNET_ASSERT(len == M_COPYALL, ("m_copym, length > size of mbuf chain"));
      break;
    }
    if (copyhdr)
      MGETHDR(n, m->m_type);
    else
      MGET(n, m->m_type);
    *np = n;
    if (n == NULL)
      goto nospace;
    if (copyhdr)
    {
      if (!m_dup_pkthdr(n, m))
        goto nospace;
      if (len == M_COPYALL)
        n->m_pkthdr.len -= off0;
      else
        n->m_pkthdr.len = len;
      copyhdr = 0;
    }
    n->m_len = min(len, m->m_len - off);
    if (m->m_flags & M_EXT)
    {
      n->m_data = m->m_data + off;
      mb_dupcl(n, m);
    }
    else
      memcpy(mtod(n, caddr_t), mtod(m, caddr_t) + off, (u_int) n->m_len);
    if (len != M_COPYALL)
      len -= n->m_len;
    off = 0;
    m = m->m_next;
    np = &n->m_next;
  }
  if (top == NULL)
    mbstat.m_mcfail++;

  return (top);
  nospace: m_freem(top);
  mbstat.m_mcfail++;
  return (NULL);
}

int m_tag_copy_chain(struct mbuf *to, struct mbuf *from)
{
  struct m_tag *p, *t, *tprev = NULL;

  ZMDNET_ASSERT(to && from,
      ("m_tag_copy_chain: null argument, to %p from %p", (void *) to, (void *) from));
  m_tag_delete_chain(to, NULL);
  SLIST_FOREACH(p, &from->m_pkthdr.tags, m_tag_link)
  {
    t = m_tag_copy(p);
    if (t == NULL)
    {
      m_tag_delete_chain(to, NULL);
      return 0;
    }
    if (tprev == NULL)
      SLIST_INSERT_HEAD(&to->m_pkthdr.tags, t, m_tag_link);
    else
      SLIST_INSERT_AFTER(tprev, t, m_tag_link);
    tprev = t;
  }
  return 1;
}

/*
 * Duplicate "from"'s mbuf pkthdr in "to".
 * "from" must have M_PKTHDR set, and "to" must be empty.
 * In particular, this does a deep copy of the packet tags.
 */
int m_dup_pkthdr(struct mbuf *to, struct mbuf *from)
{

  ZMDNET_ASSERT(to, ("m_dup_pkthdr: to is NULL"));
  ZMDNET_ASSERT(from, ("m_dup_pkthdr: from is NULL"));
  to->m_flags = (from->m_flags & M_COPYFLAGS) | (to->m_flags & M_EXT);
  if ((to->m_flags & M_EXT) == 0)
    to->m_data = to->m_pktdat;
  to->m_pkthdr = from->m_pkthdr;
  SLIST_INIT(&to->m_pkthdr.tags);
  return (m_tag_copy_chain(to, from));
}

/* Copy a single tag. */
struct m_tag *
m_tag_copy(struct m_tag *t)
{
  struct m_tag *p;

  ZMDNET_ASSERT(t, ("m_tag_copy: null tag"));
  p = m_tag_alloc(t->m_tag_cookie, t->m_tag_id, t->m_tag_len);
  if (p == NULL)
    return (NULL);
  memcpy(p + 1, t + 1, t->m_tag_len); /* Copy the data */
  return p;
}

/* Get a packet tag structure along with specified data following. */
struct m_tag *
m_tag_alloc(u_int32_t cookie, int type, int len)
{
  struct m_tag *t;

  if (len < 0)
    return NULL;
  t = malloc(len + sizeof(struct m_tag));
  if (t == NULL)
    return NULL;
  m_tag_setup(t, cookie, type, len);
  t->m_tag_free = m_tag_free_default;
  return t;
}

/* Free a packet tag. */
void m_tag_free_default(struct m_tag *t)
{
  free(t);
}

/*
 * Copy data from a buffer back into the indicated mbuf chain,
 * starting "off" bytes from the beginning, extending the mbuf
 * chain if necessary.
 */
void m_copyback(struct mbuf *m0, int off, int len, caddr_t cp)
{
  int mlen;
  struct mbuf *m = m0, *n;
  int totlen = 0;

  if (m0 == NULL)
    return;
  while (off > (mlen = m->m_len))
  {
    off -= mlen;
    totlen += mlen;
    if (m->m_next == NULL)
    {
      n = m_get(m->m_type);
      if (n == NULL)
        goto out;
      memset(mtod(n, caddr_t), 0, MLEN);
      n->m_len = min(MLEN, len + off);
      m->m_next = n;
    }
    m = m->m_next;
  }
  while (len > 0)
  {
    mlen = min(m->m_len - off, len);
    memcpy(off + mtod(m, caddr_t), cp, (u_int) mlen);
    cp += mlen;
    len -= mlen;
    mlen += off;
    off = 0;
    totlen += mlen;
    if (len == 0)
      break;
    if (m->m_next == NULL)
    {
      n = m_get(m->m_type);
      if (n == NULL)
        break;
      n->m_len = min(MLEN, len);
      m->m_next = n;
    }
    m = m->m_next;
  }
  out: if (((m = m0)->m_flags & M_PKTHDR) && (m->m_pkthdr.len < totlen))
    m->m_pkthdr.len = totlen;
}

/*
 * Lesser-used path for M_PREPEND:
 * allocate new mbuf to prepend to chain,
 * copy junk along.
 */
struct mbuf *
m_prepend(struct mbuf *m, int len, int how)
{
  struct mbuf *mn;

  if (m->m_flags & M_PKTHDR)
    MGETHDR(mn, m->m_type);
  else
    MGET(mn, m->m_type);
  if (mn == NULL)
  {
    m_freem(m);
    return (NULL);
  }
  if (m->m_flags & M_PKTHDR)
    M_MOVE_PKTHDR(mn, m);
  mn->m_next = m;
  m = mn;
  if (m->m_flags & M_PKTHDR)
  {
    if (len < MHLEN)
      MH_ALIGN(m, len);
  }
  else
  {
    if (len < MLEN)
      M_ALIGN(m, len);
  }
  m->m_len = len;
  return (m);
}

/*
 * Copy data from an mbuf chain starting "off" bytes from the beginning,
 * continuing for "len" bytes, into the indicated buffer.
 */
void m_copydata(const struct mbuf *m, int off, int len, caddr_t cp)
{
  u_int count;

  ZMDNET_ASSERT(off >= 0, ("m_copydata, negative off %d", off));
  ZMDNET_ASSERT(len >= 0, ("m_copydata, negative len %d", len));
  while (off > 0)
  {
    ZMDNET_ASSERT(m != NULL, ("m_copydata, offset > size of mbuf chain"));
    if (off < m->m_len)
      break;
    off -= m->m_len;
    m = m->m_next;
  }
  while (len > 0)
  {
    ZMDNET_ASSERT(m != NULL, ("m_copydata, length > size of mbuf chain"));
    count = min(m->m_len - off, len);
    memcpy(cp, mtod(m, caddr_t) + off, count);
    len -= count;
    cp += count;
    off = 0;
    m = m->m_next;
  }
}

/*
 * Concatenate mbuf chain n to m.
 * Both chains must be of the same type (e.g. MT_DATA).
 * Any m_pkthdr is not updated.
 */
void m_cat(struct mbuf *m, struct mbuf *n)
{
  while (m->m_next)
    m = m->m_next;
  while (n)
  {
    if ((m->m_flags & M_EXT)
        || m->m_data + m->m_len + n->m_len >= &m->m_dat[MLEN])
    {
      /* just join the two chains */
      m->m_next = n;
      return;
    }
    /* splat the data from one into the other */
    memcpy(mtod(m, caddr_t) + m->m_len, mtod(n, caddr_t), (u_int) n->m_len);
    m->m_len += n->m_len;
    n = m_free(n);
  }
}

void m_adj(struct mbuf *mp, int req_len)
{
  int len = req_len;
  struct mbuf *m;
  int count;

  if ((m = mp) == NULL)
    return;
  if (len >= 0)
  {
    /*
     * Trim from head.
     */
    while (m != NULL && len > 0)
    {
      if (m->m_len <= len)
      {
        len -= m->m_len;
        m->m_len = 0;
        m = m->m_next;
      }
      else
      {
        m->m_len -= len;
        m->m_data += len;
        len = 0;
      }
    }
    m = mp;
    if (mp->m_flags & M_PKTHDR)
      m->m_pkthdr.len -= (req_len - len);
  }
  else
  {
    /*
     * Trim from tail.  Scan the mbuf chain,
     * calculating its length and finding the last mbuf.
     * If the adjustment only affects this mbuf, then just
     * adjust and return.  Otherwise, rescan and truncate
     * after the remaining size.
     */
    len = -len;
    count = 0;
    for (;;)
    {
      count += m->m_len;
      if (m->m_next == (struct mbuf *) 0)
        break;
      m = m->m_next;
    }
    if (m->m_len >= len)
    {
      m->m_len -= len;
      if (mp->m_flags & M_PKTHDR)
        mp->m_pkthdr.len -= len;
      return;
    }
    count -= len;
    if (count < 0)
      count = 0;
    /*
     * Correct length for chain is "count".
     * Find the mbuf with last data, adjust its length,
     * and toss data from remaining mbufs on chain.
     */
    m = mp;
    if (m->m_flags & M_PKTHDR)
      m->m_pkthdr.len = count;
    for (; m; m = m->m_next)
    {
      if (m->m_len >= count)
      {
        m->m_len = count;
        if (m->m_next != NULL)
        {
          m_freem(m->m_next);
          m->m_next = NULL;
        }
        break;
      }
      count -= m->m_len;
    }
  }
}

/* m_split is used within zmdnet_handle_cookie_echo. */

/*
 * Partition an mbuf chain in two pieces, returning the tail --
 * all but the first len0 bytes.  In case of failure, it returns NULL and
 * attempts to restore the chain to its original state.
 *
 * Note that the resulting mbufs might be read-only, because the new
 * mbuf can end up sharing an mbuf cluster with the original mbuf if
 * the "breaking point" happens to lie within a cluster mbuf. Use the
 * M_WRITABLE() macro to check for this case.
 */
struct mbuf *
m_split(struct mbuf *m0, int len0, int wait)
{
  struct mbuf *m, *n;
  u_int len = len0, remain;

  /* MBUF_CHECKSLEEP(wait); */
  for (m = m0; m && (int) len > m->m_len; m = m->m_next)
    len -= m->m_len;
  if (m == NULL)
    return (NULL);
  remain = m->m_len - len;
  if (m0->m_flags & M_PKTHDR)
  {
    MGETHDR(n, m0->m_type);
    if (n == NULL)
      return (NULL);
    n->m_pkthdr.rcvif = m0->m_pkthdr.rcvif;
    n->m_pkthdr.len = m0->m_pkthdr.len - len0;
    m0->m_pkthdr.len = len0;
    if (m->m_flags & M_EXT)
      goto extpacket;
    if (remain > MHLEN)
    {
      /* m can't be the lead packet */
      MH_ALIGN(n, 0);
      n->m_next = m_split(m, len, wait);
      if (n->m_next == NULL)
      {
        (void) m_free(n);
        return (NULL);
      }
      else
      {
        n->m_len = 0;
        return (n);
      }
    }
    else
      MH_ALIGN(n, remain);
  }
  else if (remain == 0)
  {
    n = m->m_next;
    m->m_next = NULL;
    return (n);
  }
  else
  {
    MGET(n, m->m_type);
    if (n == NULL)
      return (NULL);
    M_ALIGN(n, remain);
  }
  extpacket: if (m->m_flags & M_EXT)
  {
    n->m_data = m->m_data + len;
    mb_dupcl(n, m);
  }
  else
  {
    memcpy(mtod(n, caddr_t), mtod(m, caddr_t) + len, remain);
  }
  n->m_len = remain;
  m->m_len = len;
  n->m_next = m->m_next;
  m->m_next = NULL;
  return (n);
}

int pack_send_buffer(caddr_t buffer, struct mbuf* mb)
{

  int count_to_copy;
  int total_count_copied = 0;
  int offset = 0;

  do
  {
    count_to_copy = mb->m_len;
    memcpy(buffer + offset, mtod(mb, caddr_t), count_to_copy);
    offset += count_to_copy;
    total_count_copied += count_to_copy;
    mb = mb->m_next;
  } while (mb);

  return (total_count_copied);
}
