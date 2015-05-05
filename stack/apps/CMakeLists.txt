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

include(${CMAKE_SOURCE_DIR}/cmake/app_macros.cmake)

#Add the 'BUILD_APPLICATIONS' option
OPTION(BUILD_APPLICATIONS "Build Applications" ON)
LIST_SUBDIRS(CUR_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
#Add conditional options for all subdirs (which are individual applications)
FOREACH(__dir ${CUR_DIRS})
    STRING(TOUPPER ${__dir} __upper_name)
    SETOPTION_IF(APP_${__upper_name} "Build Application ${__dir}" OFF BUILD_APPLICATIONS)
    UNSET(__upper_name)
ENDFOREACH()

#Hide the parameters declared by the applications during the previous run
#Applications that are enabled will re-enable their parameters during declaration (eg by calling APP_PARAM)
HIDE_PARAMETERS(APP_PARAM_LIST)

#Finally Load the individual applications
FOREACH(__dir ${CUR_DIRS})
    GEN_PREFIX(APP_PREFIX "APP" ${__dir})
    IF(BUILD_APPLICATIONS AND ${APP_PREFIX})
	ADD_SUBDIRECTORY(${__dir})
    ENDIF()	
    UNSET(APP_PREFIX)
ENDFOREACH()