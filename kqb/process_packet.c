/* Copyright 2020 sekigon-gonnoc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

#include "process_packet.h"
#include "report_descriptor_parser.h"
#include "report_parser.h"


#include "print.h"
#include "string.h"
#include "uart.h"
#include "quantum.h"
#include "pointing_device.h"

#ifndef QUANTIZER_REPORT_PARSER
#    define QUANTIZER_REPORT_PARSER REPORT_PARSER_DEFAULT
#endif

#ifndef TAP_CODE_DELAY
#    define TAP_CODE_DELAY 0
#endif

// #define NO_PRINT

bool ch559_update_mode = false;
uint32_t ch559_version = 0;

bool    ch559_start  = false;
uint8_t device_cnt   = 0;
uint8_t hid_info_cnt = 0;

enum {
    SLIP_END     = 0xC0,
    SLIP_ESC     = 0xDB,
    SLIP_ESC_END = 0xDC,
    SLIP_ESC_ESC = 0xDD,
};

enum {
    CONNECTED = 1,
    DISCONNECTED,
    ERROR,
    DEVICE_POLL,
    DEVICE_STRING,
    DEVICE_INFO,
    HID_INFO,
    STARTUP,
} msg_type;

#define SERIAL_BUFFER_LEN 1024

bool          matrix_has_changed = false;
matrix_row_t* matrix_dest;
bool          parse_packet(uint8_t* buf, uint32_t cnt, matrix_row_t* current_matrix) {
    static uint8_t         pre_keyreport[8];
    uint16_t               msg_len       = buf[LEN_L] | ((uint16_t)buf[LEN_H] << 8);
    packet_header_t const* packet_header = (packet_header_t const*)buf;

    matrix_has_changed = false;
    matrix_dest        = current_matrix;
    // dprintf("Packet received:%d\n", msg_len);

    // validate packet length
    if (cnt < REPORT_START || msg_len != cnt - REPORT_START) {
        matrix_has_changed = false;
        return matrix_has_changed;
    }

#ifndef NO_PRINT
    if (debug_enable) {
        // print received packet in hex format
        printf("Receive: device:%d, ep:%d\n", packet_header->dev_num, packet_header->ep_num);
        for (int idx = 0; idx < cnt; idx++) {
            printf("%02X ", buf[idx]);
        }
        printf("\n");
    }
#endif

    switch (buf[MSG_TYP]) {
        case STARTUP:
            dprintf("CH559 start\n");
            ch559_start = true;
            ch559_version = (packet_header->dev_type << 16) |
                            (packet_header->dev_num << 8) |
                            (packet_header->ep_num);
            break;

        case CONNECTED:
            device_cnt++;
            dprintf("Connected\n");
            break;

        case DISCONNECTED:
            device_cnt--;
            dprintf("Disconnected\n");
            for (uint8_t rowIdx = 0; rowIdx < MATRIX_ROWS; rowIdx++) {
                if (current_matrix[rowIdx] != 0) {
                    matrix_has_changed     = true;
                    current_matrix[rowIdx] = 0;
                }
                memset(pre_keyreport, 0, sizeof(pre_keyreport));
            }
#if QUANTIZER_REPORT_PARSER == REPORT_PARSER_USER
            on_disconnect_device_user(packet_header->dev_type);
#endif
            break;

        case HID_INFO:
            dprintf("Receive HID info\n");
#if QUANTIZER_REPORT_PARSER == REPORT_PARSER_DEFAULT
            // ch559 send dev_num in dev_type offset for this type packet
            parse_report_descriptor(packet_header->dev_type, &packet_header->data_start, packet_header->len);
#elif QUANTIZER_REPORT_PARSER == REPORT_PARSER_FIXED
            // no descriptor parser (Fixed)
#elif QUANTIZER_REPORT_PARSER == REPORT_PARSER_USER
            report_descriptor_parser_user(packet_header->dev_type, &packet_header->data_start, packet_header->len);
#else
#    error "Unknwon report parser"
#endif
            break;

        case DEVICE_POLL:
#if QUANTIZER_REPORT_PARSER == REPORT_PARSER_DEFAULT
            matrix_has_changed = parse_report(packet_header->dev_num, &packet_header->data_start, packet_header->len);
#elif QUANTIZER_REPORT_PARSER == REPORT_PARSER_FIXED
            matrix_has_changed = report_parser_fixed(buf, msg_len, pre_keyreport, current_matrix);
#elif QUANTIZER_REPORT_PARSER == REPORT_PARSER_USER
            matrix_has_changed = report_parser_user(buf, cnt, current_matrix);
#endif
            break;
    }

    return matrix_has_changed;
}

bool process_packet(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;
    bool receive_complete   = false;

    static uint8_t  buf[SERIAL_BUFFER_LEN];  // serial buffer
    static uint16_t widx     = 0;            // write index
    static bool     escaped  = false;        // escape flag
    static bool     overflow = false;        // overflow flag

    if (ch559_update_mode) {
        return false;
    }

    // process all available packet
    while (uart_available()) {
        while (uart_available()) {
            uint8_t c = uart_getchar();

            // process SLIP
            if (c == SLIP_END) {
                // dprintf("Detect END signal\n");
                if (overflow) {
                    // reset receive buffer
                    overflow         = false;
                    receive_complete = false;
                    widx             = 0;
                    escaped          = false;
                    memset(buf, 0, sizeof(buf));
                } else {
                    receive_complete = true;
                }
                break;
            } else if (c == SLIP_ESC) {
                escaped = true;
            } else if (widx < sizeof(buf)) {
                if (escaped) {
                    if (c == SLIP_ESC_END) {
                        buf[widx] = SLIP_END;
                    } else if (c == SLIP_ESC_ESC) {
                        buf[widx] = SLIP_ESC;
                    } else {
                        buf[widx] = c;
                    }
                    escaped = false;
                } else {
                    buf[widx] = c;
                }

                widx++;
                if (widx > sizeof(buf)) {
                    dprintf("Buffer overflow\n");
                    overflow = true;
                    widx     = 0;
                }
            }
        }

        if (receive_complete) {
            matrix_has_changed = parse_packet(buf, widx, current_matrix);

            receive_complete = false;
            widx             = 0;
            escaped          = false;
            overflow         = false;
            memset(buf, 0, sizeof(buf));
        }
    }

    return matrix_has_changed;
}
