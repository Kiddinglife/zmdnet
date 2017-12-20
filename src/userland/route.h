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

#ifndef __ZMDNET_ROUTE_H__
#define __ZMDNET_ROUTE_H__

#include "portable.h"

// todo this is not actully used mtx_init() and mtx_destroy() are not implemented
#define mtx_lock(arg1)
#define mtx_unlock(arg1)
#define mtx_assert(arg1,arg2)

#define RT_LOCK_INIT(_rt) mtx_init(&(_rt)->rt_mtx, "rtentry", NULL, MTX_DEF | MTX_DUPOK)
#define RT_LOCK(_rt) mtx_lock(&(_rt)->rt_mtx)
#define RT_UNLOCK(_rt) mtx_unlock(&(_rt)->rt_mtx)
#define RT_LOCK_DESTROY(_rt)	mtx_destroy(&(_rt)->rt_mtx)
#define RT_LOCK_ASSERT(_rt)	mtx_assert(&(_rt)->rt_mtx, MA_OWNED)

#define RT_ADDREF(_rt) RT_LOCK_ASSERT(_rt); zmdnet_assert((_rt)->rt_refcnt >= 0,("negative refcnt %ld", (_rt)->rt_refcnt)); (_rt)->rt_refcnt++;	
#define RT_REMREF(_rt) RT_LOCK_ASSERT(_rt);	zmdnet_assert((_rt)->rt_refcnt > 0,("bogus refcnt %ld", (_rt)->rt_refcnt));	(_rt)->rt_refcnt--;	

#define RTFREE_LOCKED(_rt) if ((_rt)->rt_refcnt <= 1)  rtfree(_rt); else RT_REMREF(_rt); RT_UNLOCK(_rt); _rt = NULL; /* guard against invalid refs */		
#define RTFREE(_rt) RT_LOCK(_rt);RTFREE_LOCKED(_rt);

#define rtalloc(ro, vrf_id, fibnum) zmdnet_rtalloc((sctp_route_t *)ro)
#define rtfree(arg1) zmdnet_rtfree(arg1)

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

/*
* Kernel resident routing tables.
* The routing tables are initialized when interface addresses
* are set by making entries for all directly connected interfaces.
* A route consists of a destination address and a reference
* to a routing entry.  These are often held by protocols
* in their control blocks, e.g. inpcb.
*/
struct zmdnet_route
{
    struct zmdnet_rtentry *ro_rt;
    struct sockaddr ro_dst;
};

/*
* These numbers are used by reliable protocols for determining
* retransmission behavior and are included in the routing structure.
*/
struct rt_metrics_lite
{
    uint32_t rmx_mtu;	/* MTU for this path */
    u_long	rmx_expire;	/* lifetime for route, e.g. redirect */
    u_long	rmx_pksent;	/* packets sent using this route */
};

struct zmdnet_rtentry
{
#if 0
    struct	radix_node rt_nodes[2];	/* tree glue, and other values */
                                    /*
                                    * XXX struct rtentry must begin with a struct radix_node (or two!)
                                    * because the code does some casts of a 'struct radix_node *'
                                    * to a 'struct rtentry *'
                                    */
#define	rt_key(r)	(*((struct sockaddr **)(&(r)->rt_nodes->rn_key)))
#define	rt_mask(r)	(*((struct sockaddr **)(&(r)->rt_nodes->rn_mask)))
    struct	sockaddr *rt_gateway;	/* value */
    u_long	rt_flags;		/* up/down?, host/net */
#endif
    struct	ifnet *rt_ifp;		/* the answer: interface to use */
    struct	ifaddr *rt_ifa;		/* the answer: interface address to use */
    struct	rt_metrics_lite rt_rmx;	/* metrics used by rx'ing protocols */
    long	rt_refcnt;		/* # held references */
#if 0
    struct	sockaddr *rt_genmask;	/* for generation of cloned routes */
    caddr_t	rt_llinfo;		/* pointer to link level info cache */
    struct	rtentry *rt_gwroute;	/* implied entry for gatewayed routes */
    struct	rtentry *rt_parent; 	/* cloning parent of this route */
#endif
    struct	mtx rt_mtx;		/* mutex for routing entry */
};

static inline void zmdnet_rtalloc(struct zmdnet_route* ro)
{
    if (ro->ro_rt != NULL)
    {
        ro->ro_rt->rt_refcnt++;
        return;
    }
    zmdnet_malloc_zero(ro->ro_rt, struct zmdnet_rtentry, sizeof(struct zmdnet_rtentry));
    ro->ro_rt->rt_refcnt = 1;
    /* set MTU TODO set this based on the ro->ro_dst, looking up MTU with routing socket */
#if 0
    if (userspace_rawroute == -1)
    {
        userspace_rawroute = socket(AF_ROUTE, SOCK_RAW, 0);
        if (userspace_rawroute == -1)
            return;
    }
#endif
    ro->ro_rt->rt_rmx.rmx_mtu = 1500; /* FIXME temporary solution */
    /* TODO enable the ability to obtain interface index of route for
    *  ZMDNET_GET_IF_INDEX_FROM_ROUTE macro.
    */
}
static inline void zmdnet_rtfree(struct zmdnet_rtentry* rt)
{
    if (rt == NULL)
        return;
    if (--rt->rt_refcnt > 0)
        return;
    zmdnet_free(rt);
}

#endif
