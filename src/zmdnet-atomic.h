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

#ifndef _ZMDNET_ATOMIC_H_
#define _ZMDNET_ATOMIC_H_

#include "config.h"

#include <stdio.h>
#include <sys/types.h>

#if defined(DARWIN) || defined (_WIN32)
#if defined (_WIN32)
#define atomic_add_int(addr, val) InterlockedExchangeAdd((LPLONG)addr, (LONG)val)
#define atomic_fetchadd_int(addr, val) InterlockedExchangeAdd((LPLONG)addr, (LONG)val)
#define atomic_subtract_int(addr, val)   InterlockedExchangeAdd((LPLONG)addr,-((LONG)val))
#define atomic_cmpset_int(dst, exp, src) InterlockedCompareExchange((LPLONG)dst, src, exp)
#define ZMDNET_DECREMENT_AND_CHECK_REFCOUNT(addr) (InterlockedExchangeAdd((LPLONG)addr, (-1L)) == 1)
#else
#include <libkern/OSAtomic.h>
#define atomic_add_int(addr, val) OSAtomicAdd32Barrier(val, (int32_t *)addr)
#define atomic_fetchadd_int(addr, val) OSAtomicAdd32Barrier(val, (int32_t *)addr)
#define atomic_subtract_int(addr, val) OSAtomicAdd32Barrier(-val, (int32_t *)addr)
#define atomic_cmpset_int(dst, exp, src) OSAtomicCompareAndSwapIntBarrier(exp, src, (int *)dst)
#define ZMDNET_DECREMENT_AND_CHECK_REFCOUNT(addr) (atomic_fetchadd_int(addr, -1) == 0)
#endif

#if defined(RUN_TIME_CHECKS)
#define ZMDNET_SAVE_ATOMIC_DECREMENT(addr, val) \
{ \
	int32_t newval; \
	newval = atomic_fetchadd_int(addr, -val); \
	if (newval < 0) { \
		panic("Counter goes negative"); \
	} \
}
#else
#define ZMDNET_SAVE_ATOMIC_DECREMENT(addr, val) \
{ \
	int32_t newval; \
	newval = atomic_fetchadd_int(addr, -val); \
	if (newval < 0) { \
		*addr = 0; \
	} \
}
#endif

#if defined(_WIN32)
static void atomic_init()
{} /* empty when we are not using atomic_mtx */
#else
static inline void atomic_init()
{} /* empty when we are not using atomic_mtx */
#endif

#else //!( defined(DARWIN) || defined (_WIN32))

/* Using gcc built-in functions for atomic memory operations
 Reference: http://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html
 Requires gcc version 4.1.0
 compile with -march=i486
 */

/*Atomically add V to *P.*/
#define atomic_add_int(P, V)	 (void) __sync_fetch_and_add(P, V)

/*Atomically subtrace V from *P.*/
#define atomic_subtract_int(P, V) (void) __sync_fetch_and_sub(P, V)

/*
 * Atomically add the value of v to the integer pointed to by p and return
 * the previous value of *p.
 */
#define atomic_fetchadd_int(p, v) __sync_fetch_and_add(p, v)

/* Following explanation from src/sys/i386/include/atomic.h,
 * for atomic compare and set
 *
 * if (*dst == exp) *dst = src (all 32 bit words)
 *
 * Returns 0 on failure, non-zero on success
 */

#define atomic_cmpset_int(dst, exp, src) __sync_bool_compare_and_swap(dst, exp, src)

#define ZMDNET_DECREMENT_AND_CHECK_REFCOUNT(addr) (atomic_fetchadd_int(addr, -1) == 1)
#if defined(RUN_TIME_CHECKS)
#define ZMDNET_SAVE_ATOMIC_DECREMENT(addr, val) \
{ \
	int32_t oldval; \
	oldval = atomic_fetchadd_int(addr, -val); \
	if (oldval < val) { \
		panic("Counter goes negative"); \
	} \
}
#else
#define ZMDNET_SAVE_ATOMIC_DECREMENT(addr, val) \
{ \
	int32_t oldval; \
	oldval = atomic_fetchadd_int(addr, -val); \
	if (oldval < val) { \
		*addr = 0; \
	} \
}
#endif
static inline void atomic_init()
{
} /* empty when we are not using atomic_mtx */
#endif

#endif
