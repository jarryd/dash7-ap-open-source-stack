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

SET(CC1101_SRC
        cc1101.c
        cc1101.h
        cc1101_interface.c
)

# interface specific logic
IF(PLATFORM STREQUAL "wizzimote" OR PLATFORM STREQUAL "uocmote") # TODO detect all CC430 based platform instead of only wizzimote
    INCLUDE_DIRECTORIES(${TOOLCHAIN_DIR}/include)
    LIST(APPEND CC1101_SRC cc1101_interface_cc430.c)
ELSE()
    LIST(APPEND CC1101_SRC cc1101_interface_spi.c)
ENDIF()

#An object library with name '${CHIP_LIBRARY_NAME}' MUST be generated by the CMakeLists.txt file for every chip
ADD_LIBRARY (${CHIP_LIBRARY_NAME} OBJECT ${CC1101_SRC})
