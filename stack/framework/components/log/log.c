/* * OSS-7 - An opensource implementation of the DASH7 Alliance Protocol for ultra
 * lowpower wireless sensor communication
 *
 * Copyright 2015 University of Antwerp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*! \file log.c
 * \author maarten.weyn@uantwerpen.be
 * \author glenn.ergeerts@uantwerpen.be
 * \author daniel.vandenakker@uantwerpen.be
 */

#include "log.h"
#include "string.h"
#include "ng.h"
#include "hwuart.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <hwradio.h>
#include "framework_defs.h"

#ifdef FRAMEWORK_LOG_ENABLED

typedef enum {
    LOG_TYPE_STRING = 0x01,
    LOG_TYPE_DATA = 0x02,
    LOG_TYPE_STACK = 0x03,
    LOG_TYPE_PHY_PACKET_TX = 0X04,
    LOG_TYPE_PHY_PACKET_RX = 0X05
} log_type_t;

#ifdef FRAMEWORK_LOG_BINARY
	#define BUFFER_SIZE 100
	static char NGDEF(buffer)[BUFFER_SIZE];
#else
	static uint32_t NGDEF(counter);
#endif //FRAMEWORK_LOG_BINARY

__LINK_C void log_counter_reset()
{
#ifndef FRAMEWORK_LOG_BINARY
	NG(counter) = 0;
#endif //FRAMEWORK_LOG_BINARY

}

__LINK_C void log_print_string(char* format, ...)
{
    va_list args;
    va_start(args, format);
#ifdef FRAMEWORK_LOG_BINARY
    uint8_t len = vsnprintf(NG(buffer), BUFFER_SIZE, format, args);
    uart_transmit_data(0xDD);
    uart_transmit_data(LOG_TYPE_STRING);
    uart_transmit_data(len);
    uart_transmit_message(NG(buffer),len);
#else
    printf("\n\r[%03d] ", NG(counter)++);
    vprintf(format, args);
#endif //FRAMEWORK_LOG_BINARY
    va_end(args);
}

__LINK_C void log_print_stack_string(log_stack_layer_t type, char* format, ...)
{
    va_list args;
    va_start(args, format);
#ifdef FRAMEWORK_LOG_BINARY
    uint8_t len = vsnprintf(NG(buffer), BUFFER_SIZE, format, args);
    uart_transmit_data(0xDD);
    uart_transmit_data(LOG_TYPE_STACK);
    uart_transmit_data(type);
    uart_transmit_data(len);
    uart_transmit_message(NG(buffer),len);
#else
    printf("\n\r[%03d] ", NG(counter)++);
    vprintf(format, args);
#endif //FRAMEWORK_LOG_BINARY
    va_end(args);
}

__LINK_C void log_print_data(uint8_t* message, uint32_t length)
{
#ifdef FRAMEWORK_LOG_BINARY
    uart_transmit_data(0xDD);
    uart_transmit_data(LOG_TYPE_DATA);
    uart_transmit_data(length);
    uart_transmit_message(message, length);
#else
    printf("\n\r[%03d]", NG(counter)++);
    for( uint32_t i=0 ; i<length ; i++ )
    {
        printf(" %02X", message[i]);
    }
#endif //FRAMEWORK_LOG_BINARY
}

__LINK_C void log_print_raw_phy_packet(hw_radio_packet_t* packet, bool is_tx)
{
#ifdef FRAMEWORK_LOG_BINARY
    uart_transmit_data(0xDD);
    if(is_tx) {

        uart_transmit_data(LOG_TYPE_PHY_PACKET_TX);

        uart_transmit_message(&(packet->tx_meta.timestamp), sizeof(timer_tick_t));

        uart_transmit_data(packet->tx_meta.tx_cfg.channel_id.channel_header_raw);

        uart_transmit_data(packet->tx_meta.tx_cfg.channel_id.center_freq_index);

        uart_transmit_data(packet->tx_meta.tx_cfg.syncword_class);

        uart_transmit_data(packet->tx_meta.tx_cfg.eirp);

        uart_transmit_message(packet->data, packet->length+1);

    } else {
        uart_transmit_data(LOG_TYPE_PHY_PACKET_RX);
        uart_transmit_message(&(packet->rx_meta.timestamp), sizeof(timer_tick_t));
        uart_transmit_data(packet->rx_meta.rx_cfg.channel_id.channel_header_raw);
        uart_transmit_data(packet->rx_meta.rx_cfg.channel_id.center_freq_index);
        uart_transmit_data(packet->rx_meta.rx_cfg.syncword_class);
        uart_transmit_data(packet->rx_meta.lqi);
        uart_transmit_message(&(packet->rx_meta.rssi), sizeof(int16_t));
        // TODO CRC?
        uart_transmit_message(packet->data, packet->length+1);
    }
#endif // FRAMEWORK_LOG_BINARY
}

#endif //FRAMEWORK_LOG_ENABLED
