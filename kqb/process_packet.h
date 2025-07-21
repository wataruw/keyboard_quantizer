
#pragma once

#include "quantum.h"

#define REPORT_PARSER_DEFAULT 1 /* Parse report descriptor based on simple subset */
#define REPORT_PARSER_FIXED   2 /* Small size, only boot keyboard report is acceptable */
#define REPORT_PARSER_USER 3    /* Use user defined parser */
#define REPORT_PARSER_BMP 4     /* Use BMP parser */

typedef enum {
    LEN_L = 0,
    LEN_H,
    MSG_TYP,
    DEV_TYP,
    DEV_NUM,
    EP,
    VID_L,
    VID_H,
    PID_L,
    PID_H,
    REPORT_START,
} packet_index_t;

typedef enum {
    NONE = 0,
    POINTER,
    MOUSE,
    RESERVED,
    JOYSTICK,
    GAMEPAD,
    KEYBOARD,
    KEYPAD,
    MULTI_AXIS,
    SYSTEM,
} dev_type_t;

typedef struct {
    uint16_t len;
    uint8_t  msg_type;
    uint8_t  dev_type;
    uint8_t  dev_num;
    uint8_t  ep_num;
    uint16_t vid;
    uint16_t pid;
    uint8_t  data_start;
} __attribute__((packed)) packet_header_t;

bool report_parser_fixed(uint8_t const* buf, uint8_t msg_len, uint8_t* pre_keyreport, matrix_row_t* current_matrix);
void report_descriptor_parser_user(uint8_t dev_num, uint8_t const * buf, uint16_t len);
bool report_parser_user(uint8_t const * buf, uint16_t len, matrix_row_t * current_matrix);
void on_disconnect_device_user(uint8_t device);

bool process_packet(matrix_row_t current_matrix[]);
