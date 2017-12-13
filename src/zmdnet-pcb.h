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

#ifndef __ZMDNET_PCB_H__
#define __ZMDNET_PCB_H__

#include "zmdnet-constant.h"

#if defined(__FreeBSD__) && __FreeBSD_version >= 801000
VNET_DECLARE(struct zmdnet_base_info, g_base_info) ;
#else
extern struct zmdnet_base_info g_base_info;
#endif

/* All static structures that
 * anchor the system must be here.
 */
struct zmdnet_base_info
{
    void (*debug_printf_cb)(const char *format, ...);
};

/*-
 * Here we have all the relevant information for each SCTP entity created. We
 * will need to modify this as approprate. We also need to figure out how to
 * access /dev/random.
 */
struct zmdnet_pcb
{
  unsigned int time_of_secret_change; /* number of seconds from timeval.tv_sec */
  unsigned int secret_key[SCTP_HOW_MANY_SECRETS][SCTP_NUMBER_OF_SECRETS];
  unsigned int size_of_a_cookie;
};
#endif
