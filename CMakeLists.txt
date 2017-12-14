#
# Copyright (C) 2015-2017 mengdi zhang
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the project nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

cmake_minimum_required(VERSION 3.0)
project(zmdnetlib)

# Debug build type as default
if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, using DEBUG")
	set(CMAKE_BUILD_TYPE "DEBUG")
endif ()

include(CheckStructHasMember)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckCCompilerFlag)

#################################################
# CHECK OPTIONS
#################################################

option(RUN_TIME_CHECKS "Add runtime checks" 1)
if (RUN_TIME_CHECKS)
    message(STATUS "RUN_TIME_CHECKS")
    add_definitions(-DRUN_TIME_CHECKS)
endif ()

option(ZMDNET_LOCAL_TRACE_BUF "ZMDNET_LOCAL_TRACE_BUF" 1)
if (ZMDNET_LOCAL_TRACE_BUF)
    message(STATUS "ZMDNET_LOCAL_TRACE_BUF")
    add_definitions(-DZMDNET_LOCAL_TRACE_BUF)
endif ()

option(ZMDNET_LTRACE_CHUNKS "ZMDNET_LTRACE_CHUNKS" 1)
if (ZMDNET_LTRACE_CHUNKS)
    message(STATUS "ZMDNET_LTRACE_CHUNKS")
    add_definitions(-DZMDNET_LTRACE_CHUNKS)
endif ()


option(ZMDNET_LTRACE_ERRORS "ZMDNET_LTRACE_ERRORS" 1)
if (ZMDNET_LTRACE_ERRORS)
    message(STATUS "ZMDNET_LTRACE_ERRORS")
    add_definitions(-DZMDNET_LTRACE_ERRORS)
endif ()

option(ZMDNET_LOCAL_TRACE_BUF "ZMDNET_LOCAL_TRACE_BUF" 1)
if (ZMDNET_LOCAL_TRACE_BUF)
    message(STATUS "ZMDNET_LOCAL_TRACE_BUF")
    add_definitions(-DZMDNET_LOCAL_TRACE_BUF)
endif ()

option(RUN_TIME_CHECKS "Add runtime checks" 1)
if (RUN_TIME_CHECKS)
	message(STATUS "RUN_TIME_CHECKS")
	add_definitions(-DRUN_TIME_CHECKS)
endif ()

option(ZMDNET_PROCESS_LEVEL_LOCKS "ZMDNET_PROCESS_LEVEL_LOCKS" 1)
if (ZMDNET_PROCESS_LEVEL_LOCKS)
    message(STATUS "ZMDNET_PROCESS_LEVEL_LOCKS")
    add_definitions(-DZMDNET_PROCESS_LEVEL_LOCKS)
endif ()

option(ZMDNET_DEBUG "Provide debug information" 1)
if (ZMDNET_DEBUG)
	message(STATUS "ZMDNET_DEBUG")
	add_definitions(-DZMDNET_DEBUG)
endif ()

option(ZMDNET_SUPPORT_IPV4 "Support IPv4" 1)
if (ZMDNET_SUPPORT_IPV4)
	message(STATUS "ZMDNET_SUPPORT_IPV4")
	add_definitions(-DZMDNET_SUPPORT_IPV4)
endif ()

option(ZMDNET_SUPPORT_IPV6 "Support IPv6" 1)
if (ZMDNET_SUPPORT_IPV6)
	message(STATUS "ZMDNET_SUPPORT_IPV6")
	add_definitions(-DZMDNET_SUPPORT_IPV6)
endif ()

option(zmdnet_werror "Thread warning as error" 1)
option(zmdnet_link_programs_static "Link example programs static" 0)
option(zmdnet_build_programs "Build example programs" 1)
option(zmdnet_build_tests "Build tests" 1)
option(zmdnet_sanitizer_address "Compile with address sanitizer" 0)
option(zmdnet_sanitizer_memory "Compile with memory sanitizer" 0)

if (zmdnet_sanitizer_address AND zmdnet_sanitizer_memory)
	message(FATAL_ERROR "Can not compile with both sanitizer options")
endif ()

if (zmdnet_link_programs_static OR WIN32)
	set(programs_link_library "zmdnet-static")
else ()
	set(programs_link_library "zmdnet")
endif ()


#################################################
# CHECK FOR TYPES AND FUNCTIONS
#################################################

check_include_file("sys/filedesc.h" peeloff_sockopt)
if (peeloff_sockopt)
    message(STATUS "HAVE_PEELOFF_SOCKOPT")
    option(have_peeloff_sockopt "have_peeloff_sockopt" 1)
    add_definitions(-DHAVE_PEELOFF_SOCKOPT)
endif ()

check_include_file("sys/queue.h" HAVE_SYS_QUEUE_H)
if (HAVE_SYS_QUEUE_H)
    message(STATUS "HAVE_SYS_QUEUE_H")
    option(HAVE_SYS_QUEUE_H "HAVE_SYS_QUEUE_H" 1)
    add_definitions(-DHAVE_SYS_QUEUE_H)
endif ()

check_include_files("sys/socket.h;linux/if_addr.h" HAVE_LINUX_IF_ADDR_H)
if (HAVE_LINUX_IF_ADDR_H)
	message(STATUS "HAVE_LINUX_IF_ADDR_H")
	option(HAVE_LINUX_IF_ADDR_H "HAVE_LINUX_IF_ADDR_H")
	add_definitions(-DHAVE_LINUX_IF_ADDR_H)
endif ()

check_include_files("netipsec/ipsec.h;netipsec/key.h;netipsec/ipsec6.h" HAVE_IPSEC)
if (HAVE_IPSEC)
    message(STATUS "HAVE_IPSEC")
    option(HAVE_IPSEC "HAVE_IPSEC" 1)
    add_definitions(-DHAVE_IPSEC)
endif ()


check_include_files("sys/socket.h;linux/rtnetlink.h" HAVE_LINUX_RTNETLINK_H)
if (HAVE_LINUX_RTNETLINK_H)
	message(STATUS "HAVE_LINUX_RTNETLINK_H")
	option(HAVE_LINUX_RTNETLINK_H "HAVE_LINUX_RTNETLINK_H" 1)
	add_definitions(-DHAVE_LINUX_RTNETLINK_H)
endif ()

check_include_files("sys/types.h;netinet/in.h;netinet/ip.h;netinet/ip_icmp.h" HAVE_NETINET_IP_ICMP_H)
if (HAVE_NETINET_IP_ICMP_H)
	message(STATUS "HAVE_NETINET_IP_ICMP_H")
	option(HAVE_NETINET_IP_ICMP_H "HAVE_NETINET_IP_ICMP_H" 1)
	add_definitions(-DHAVE_NETINET_IP_ICMP_H)
endif ()

check_include_files("stdatomic.h" HAVE_STDATOMIC_H)
if (HAVE_STDATOMIC_H)
	message(STATUS "HAVE_STDATOMIC_H")
	option(HAVE_STDATOMIC_H "HAVE_STDATOMIC_H" 1)
	add_definitions(-DHAVE_STDATOMIC_H)
endif ()


#################################################
# CHECK STRUCT MEMBERS
#################################################

set(CMAKE_REQUIRED_INCLUDES "${CMAKE_SOURCE_DIR}/src")

check_include_file("zmdnet.h" have_zmdnet_h)
if (NOT have_zmdnet_h)
	message(FATAL_ERROR "zmdnet.h not found")
endif ()

check_struct_has_member("struct sockaddr" "sa_len" "sys/types.h;sys/socket.h" have_sa_len)
if (have_sa_len)
	message(STATUS "have_sa_len")
	option(HAVE_SA_LEN "HAVE_SA_LEN" 1)
	add_definitions(-DHAVE_SA_LEN)
endif ()


check_struct_has_member("struct sockaddr_in" "sin_len" "sys/types.h;netinet/in.h" have_sin_len)
if (have_sin_len)
	message(STATUS "have_sin_len")
	option(HAVE_SIN_LEN "DHAVE_SIN_LEN" 1)
	add_definitions(-DHAVE_SIN_LEN)
endif ()

check_struct_has_member("struct sockaddr_in6" "sin6_len" "sys/types.h;netinet/in.h" have_sin6_len)
if (have_sin6_len)
	message(STATUS "have_sin6_len")
	option(HAVE_SIN6_LEN "HAVE_SIN6_LEN" 1)
	add_definitions(-DHAVE_SIN6_LEN)
endif ()

check_struct_has_member("struct sockaddr_conn" "sconn_len" "zmdnet.h" have_sconn_len)
if (have_sconn_len)
	message(STATUS "HAVE_SCONN_LEN")
	option(HAVE_SCONN_LEN "HAVE_SCONN_LEN" 1)
	add_definitions(-DHAVE_SCONN_LEN)
endif ()


#################################################
# COMPILER SETTINGS
#################################################

# SETTINGS FOR UNIX COMPILER
if ("${CMAKE_C_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_C_COMPILER_ID}" MATCHES "AppleClang" OR "${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -std=c99 -pedantic -Wall -Wextra")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -pedantic -Wall -Wextra")

	check_c_compiler_flag(-Wfloat-equal has_wfloat_equal)
	if (has_wfloat_equal)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wfloat-equal")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal")
	endif ()

	check_c_compiler_flag(-Wshadow has_wshadow)
	if (has_wshadow)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wshadow")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
	endif ()

	check_c_compiler_flag(-Wpointer-arith has_wpointer_aritih)
	if (has_wpointer_aritih)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wpointer-arith")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
	endif ()

	check_c_compiler_flag(-Wunreachable-code has_wunreachable_code)
	if (has_wunreachable_code)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wunreachable-code")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
	endif ()

	check_c_compiler_flag(-Winit-self has_winit_self)
	if (has_winit_self)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Winit-self")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self")
	endif ()

	check_c_compiler_flag(-Wno-unused-function has_wno_unused_function)
	if (has_wno_unused_function)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-unused-function")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
	endif ()

	check_c_compiler_flag(-Wno-unused-parameter has_wno_unused_parameter)
	if (has_wno_unused_parameter)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-unused-parameter")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
	endif ()

	check_c_compiler_flag(-Wno-unreachable-code has_wno_unreachable_code)
	if (has_wno_unreachable_code)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-unreachable-code")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unreachable-code")
	endif ()

	if (zmdnet_werror)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Werror")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	endif ()

	if (zmdnet_sanitizer_address)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fsanitize=address -fno-omit-frame-pointer -g")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
	endif ()

	if (zmdnet_sanitizer_memory)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fsanitize=memory -fno-omit-frame-pointer -g -fsanitize-memory-track-origins")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fno-omit-frame-pointer -g -fsanitize-memory-track-origins")
	endif ()
endif ()

# SETTINGS FOR VISUAL STUDIO COMPILER
if ("x${CMAKE_C_COMPILER_ID}" STREQUAL "xMSVC")
	if (CMAKE_C_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	else ()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
	endif ()

	if (WERROR)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")
	endif ()
endif ()

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_REQUIRED_INCLUDES}/config.h)

#################################################
# INCLUDE SUBDIRS
#################################################
add_subdirectory(src)
if (zmdnet_build_programs)
add_subdirectory(sample)
endif()
if (zmdnet_build_tests)
add_subdirectory(test)
endif()
