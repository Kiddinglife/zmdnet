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

#ifndef __ZMDNET_MLOG_H__
#define __ZMDNET_MLOG_H__

#include "../userland/userland.h"

extern void zmdnet_print_addr(struct sockaddr *);

#if defined(ZMDNET_LOCAL_TRACE_BUF) || defined(__APPLE__)
extern void zmdnet_log_trace(uint32_t fr, const char *str, uint32_t a, uint32_t b,
    uint32_t c, uint32_t d, uint32_t e, uint32_t f);
#endif

#define panic(...) zmdnet_printf("%s(): ", __func__);zmdnet_printf(__VA_ARGS__);zmdnet_printf("\n");abort()
#if defined(RUN_TIME_CHECKS) || defined(ZMDNET_DEBUG)
#include <stdlib.h>
#define zmdnet_assert(cond,args) if (!(cond)) panic args
#else
#define zmdnet_assert(cond, args)
#endif

// todo use same var name eg. g_base_info.debug_printf_func
#define g_base_info_var(m) g_base_info.m
//SCTP_BASE_INFO
#define g_base_info_pcb_var(m) g_base_info.pcbinfo.m
//SCTP_BASE_STATS
#define g_base_info_stats g_base_info.stats
//SCTP_BASE_STAT
#define g_base_info_stats_var(m)     g_base_info.stats.m
//SCTP_BASE_SYSCTL
#define g_base_info_sysctl_var(m) g_base_info.sysctl.m

#define zmdnet_printf(...) if(g_base_info.debug_printf_func) g_base_info.debug_printf_func(__VA_ARGS__)

#if defined(ZMDNET_DEBUG)
#include "../protocolstack/constant.h"
#define zmdnet_debug(level, ...) if ((g_base_info_sysctl_var(allowed_debug_levels)) & level) {zmdnet_printf(__VA_ARGS__);}
#define zmdnet_debug_addr(level, addr) if (g_base_info_sysctl_var(allowed_debug_levels) & level ) { zmdnet_print_addr(addr);}
#else
#define zmdnet_debug(level, ...)
#define zmdnet_debug_addr(level, addr)
#endif

// ??? i think we only need one macro right? ZMDNET_LOCAL_TRACE_BUF or ZMDNET_LTRACE_CHUNKS ???
#if defined(ZMDNET_LOCAL_TRACE_BUF)
#define GET_CYCLECOUNT 0 /*get_cyclecount() // TODO use gettimeofday to get timestamp */
#define zmdnet_log_trace mlog_trace  /* <<< SCTP_CTR6 */
#else
#define zmdnet_log_trace  /* <<< SCTP_CTR6 */
#endif

#ifdef ZMDNET_LTRACE_CHUNKS
#define zmdnet_ltrace_chunk(a, b, c, d) \
if(g_base_info_sysctl_var(logging_level) & ZMDNET_LTRACE_CHUNK_ENABLE) zmdnet_log_trace(KTR_SUBSYS, "SCTP:%d[%d]:%x-%x-%x-%x", SCTP_LOG_CHUNK_PROC, 0, a, b, c, d)
#else
#define zmdnet_ltrace_chunk(a, b, c, d)
#endif

#ifdef  ZMDNET_LTRACE_ERRORS
#define zmdnet_ltrace_err_ret_pkt(m, inp, stcb, net, file, err) \
    if (g_base_info_sysctl_var(logging_level) & ZMDNET_LTRACE_ERR_ENABLE) \
        zmdnet_printf("mbuf:%p inp:%p stcb:%p net:%p file:%x line:%d error:%d\n",  (void *)m, (void *)inp, (void *)stcb, (void *)net, file, __LINE__, err);
#define zmdnet_ltrace_err_ret(inp, stcb, net, file, err) \
    if (logging_level & ZMDNET_LTRACE_ERROR_ENABLE) \
        zmdnet_printf("inp:%p stcb:%p net:%p file:%x line:%d error:%d\n",  (void *)inp, (void *)stcb, (void *)net, file, __LINE__, err);
#else
#define zmdnet_ltrace_err_ret_pkt(m, inp, stcb, net, file, err)
#define zmdnet_ltrace_err_ret(inp, stcb, net, file, err)
#endif

#define iamhere() zmdnet_printf("%s:%d at %s\n", __FILE__, __LINE__ , __func__)

/* For BSD this just accesses the M_PKTHDR length so it operates on an mbuf with hdr flag. 
 * Other O/S's may have seperate packet header and mbuf chain pointers.. thus we need macros.
*/
#define mbuf_header_to_chain(m) (m)
#define mbuf_detach_header_from_chain(m)
#define mbuf_header_len(m) ((m)->m_pkthdr.len)
#define mbuf_get_header_for_output(o_pak) 0
#define mbuf_release_header(m)
#define mbuf_release_pkt(m)	mbuf_m_freem(m)
#define mbuf_get_pkt_vrfid(m, vrf_id)  ((vrf_id = SCTP_DEFAULT_VRFID) != SCTP_DEFAULT_VRFID)
/* Attach the chain of data into the sendable packet. */
#define mbuf_attach_chain(pak, m, packet_length) pak = m; pak->m_pkthdr.len = packet_length; 

#endif
