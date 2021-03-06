# 
# OSS-7 - An opensource implementation of the DASH7 Alliance Protocol for ultra
# lowpower wireless sensor communication
#
# Copyright 2015 University of Antwerp
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#Export the interface of the framework to the rest of the code base
EXPORT_GLOBAL_INCLUDE_DIRECTORIES(inc)

#include the 'binary' platform directory so the 'framework_defs.h' file
#generated by FRAMEWORK_BUILD_SETTINGS file can be found
EXPORT_GLOBAL_INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#Include framework-specific helper MACRO's
include(${CMAKE_SOURCE_DIR}/cmake/framework_macros.cmake)

#add the hal
ADD_SUBDIRECTORY("hal")
#add the non-hal components
ADD_SUBDIRECTORY("components")

#Add framework options && import them in the framework_defs.h file
#This is NOT done seperately for each component since components can be overridden by a platform
#And these settings should not be
SET(FRAMEWORK_SCHEDULER_MAX_TASKS "16" CACHE STRING "The maximum number of tasks that can be registered with the scheduler")
FRAMEWORK_HEADER_DEFINE(NUMBER FRAMEWORK_SCHEDULER_MAX_TASKS)

SET(FRAMEWORK_SCHEDULER_LP_MODE "0" CACHE STRING "The low power mode to use. Only change this if you know exactly what you are doing")
FRAMEWORK_HEADER_DEFINE(NUMBER FRAMEWORK_SCHEDULER_LP_MODE)

SET(FRAMEWORK_LOG_BINARY "TRUE" CACHE BOOL "Use binary logging format (which can be parsed by pylogger tool)")
FRAMEWORK_HEADER_DEFINE(BOOL FRAMEWORK_LOG_BINARY)

SET(FRAMEWORK_LOG_ENABLED "TRUE" CACHE BOOL "Select whether to enable or disable the generation of logs")
FRAMEWORK_HEADER_DEFINE(BOOL FRAMEWORK_LOG_ENABLED)

SET(FRAMEWORK_DEBUG_ASSERT_MINIMAL "FALSE" CACHE BOOL "Enabling this strips file, line functin and condition information from asserts, to save ROM")
FRAMEWORK_HEADER_DEFINE(BOOL FRAMEWORK_DEBUG_ASSERT_MINIMAL)

SET(FRAMEWORK_TIMER_STACK_SIZE "10" CACHE STRING "The number of simultaneous timer events that can be scheduled. Increase this if you have lots of concurrent timers")
FRAMEWORK_HEADER_DEFINE(NUMBER FRAMEWORK_TIMER_STACK_SIZE)

SET(FRAMEWORK_TIMER_RESOLUTION "1MS" CACHE STRING "The frequency of the framework timer. One of '1MS' (1024 ticks per second) or '32K' (32768 ticks per second)")
SET_PROPERTY( CACHE FRAMEWORK_TIMER_RESOLUTION PROPERTY STRINGS "1MS;32K")
FRAMEWORK_HEADER_DEFINE(ID FRAMEWORK_TIMER_RESOLUTION)

SET(FRAMEWORK_TIMER_MODE "Reset" CACHE STRING "The operation mode of the framework timer. One of 'Normal' or 'Reset'")
SET_PROPERTY( CACHE FRAMEWORK_TIMER_MODE PROPERTY STRINGS "Normal;Reset")
IF(FRAMEWORK_TIMER_MODE STREQUAL "Reset")
    SET(FRAMEWORK_TIMER_RESET_COUNTER "TRUE")
ELSE()
    SET(FRAMEWORK_TIMER_RESET_COUNTER "FALSE")
ENDIF()
FRAMEWORK_HEADER_DEFINE(BOOL FRAMEWORK_TIMER_RESET_COUNTER)

#Generate the 'framework_defs.h'
FRAMEWORK_BUILD_SETTINGS_FILE()

SET(FRAMEWORK_SOURCES framework_bootstrap.c)
SET(FRAMEWORK_HEADERS
        inc/bootstrap.h
        inc/errors.h
        inc/link_c.h
        inc/log.h
        inc/ng.h
        inc/random.h
        inc/scheduler.h
        inc/timer.h
        inc/types.h
        inc/fifo.h
        inc/bitmap.h
        inc/debug.h
)

SET(HAL_API_HEADERS
        hal/inc/hwatomic.h
        hal/inc/hwleds.h
        hal/inc/hwgpio.h
        hal/inc/hwradio.h
        hal/inc/hwsystem.h
        hal/inc/hwdebug.h
        hal/inc/hwuart.h
        hal/inc/hwtimer.h
        hal/inc/hwspi.h
)

# Assemble the library
# PLATFORM_CHIP_LIBS: Object libraries of individual chips
# PLATFORM: Object library of the platform itself
# FRAMEWORK_OVERRIDE_LIBS: non-hal object libraries that were overridden by the HAL
# FRAMEWORK_LIBS: non-hal object libraries (not overridden)
SET(LIBNAMES ${PLATFORM_CHIP_LIBS} "PLATFORM" ${FRAMEWORK_OVERRIDE_LIBS} ${FRAMEWORK_LIBS})

#Some wizardry to get ADD_LIBRARY to accept everything
FOREACH(libname ${LIBNAMES})
    LIST(APPEND LIBS "$<TARGET_OBJECTS:${libname}>")
ENDFOREACH()
#AND finally add the library
ADD_LIBRARY(framework STATIC ${LIBS} ${FRAMEWORK_SOURCES} ${FRAMEWORK_HEADERS} ${HAL_API_HEADERS})

GET_PROPERTY(__global_include_dirs GLOBAL PROPERTY GLOBAL_INCLUDE_DIRECTORIES)
GET_PROPERTY(__platform_include_dirs GLOBAL PROPERTY PLATFORM_INCLUDE_DIRECTORIES) 
TARGET_INCLUDE_DIRECTORIES("PLATFORM" PUBLIC 
    ${__global_include_dirs}
    ${__platform_include_dirs}
)

TARGET_INCLUDE_DIRECTORIES("framework" PUBLIC  
    ${__global_include_dirs}
)

GET_PROPERTY(__global_compile_definitions GLOBAL PROPERTY GLOBAL_COMPILE_DEFINITIONS) 
TARGET_COMPILE_DEFINITIONS("PLATFORM" PUBLIC ${__global_compile_definitions})
