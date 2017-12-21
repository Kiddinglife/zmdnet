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

include(CheckIncludeFile)
find_package(Threads)
include_directories(${CMAKE_SOURCE_DIR}/src)

set(check_programs
	test-auth.c
)

#################################################
# OS DEPENDENT
#################################################
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-D_GNU_SOURCE)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	add_definitions(-U__FreeBSD__)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -Wno-address-of-packed-member")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-address-of-packed-member")
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	add_definitions(-U__APPLE__)
	add_definitions(-D__APPLE_USE_RFC_2292)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -Wno-address-of-packed-member -Wno-deprecated-declarations")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-address-of-packed-member -Wno-deprecated-declarations")
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "DragonFly")
	add_definitions(-U__DragonFly__)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "NetBSD")
	add_definitions(-U__NetBSD__)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
	add_definitions(-U__OpenBSD__)
endif ()

#################################################
# build all exe
#################################################
foreach (source_file ${check_programs})
	get_filename_component(source_file_we ${source_file} NAME_WE)
	add_executable(${source_file_we} ${source_file})
	target_link_libraries(${source_file_we} ${programs_link_library} ${CMAKE_THREAD_LIBS_INIT})
endforeach ()







