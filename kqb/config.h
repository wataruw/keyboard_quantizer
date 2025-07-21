
#pragma once

#define MATRIX_SCAN_TIME_MS 5 // Minimum is 5ms
#define BMP_FORCE_SAFE_MODE false

/* key matrix size */
#define MATRIX_ROWS_DEFAULT 32
#define MATRIX_COLS_DEFAULT 8
#define THIS_DEVICE_ROWS 32
#define THIS_DEVICE_COLS 8
#define MATRIX_MSGES_ROW 31

#define IS_LEFT_HAND true
#define BMP_DEFAULT_MODE SINGLE
#define PROCESS_MATRIX_REVERSE

#define QUANTIZER_REPORT_PARSER REPORT_PARSER_USER

#define CONFIG_RESERVED \
    { 0, 7, 0, 0, 0, 90, 0, 0 }

#define RGBLIGHT_SPLIT
#define DIODE_DIRECTION COL2ROW
#define ENCODERS_PAD_A { NO_PIN }
#define ENCODERS_PAD_B { NO_PIN }

#ifndef __ASSEMBLER__
#    include "microshell/core/msconf.h"
#    include "microshell/util/mscmd.h"
#    define USER_DEFINED_MSCMD \
        { "kqb_settings", usrcmd_kqb_settings, "Get/Set kqb settings" }, \
        { "chboot", usrcmd_chboot, "Start CH559 programming" }, \
        { "chreset", usrcmd_chreset, "Reset CH559" }, \
        { "chload", usrcmd_chload, "Current load test CH559" },\
        { "chled", usrcmd_chled, "Send LED command to CH559" },\
        { "chdev", usrcmd_chdev, "Show devices connected to CH559" },\
        { "chver", usrcmd_chver, "Show CH559 FW version" },\
        { "chprsr", usrcmd_chparser, "Change parser type" },
MSCMD_USER_RESULT usrcmd_kqb_settings(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
MSCMD_USER_RESULT usrcmd_chboot(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
MSCMD_USER_RESULT usrcmd_chreset(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
MSCMD_USER_RESULT usrcmd_chload(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
MSCMD_USER_RESULT usrcmd_chled(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
MSCMD_USER_RESULT usrcmd_chdev(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
MSCMD_USER_RESULT usrcmd_chver(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
MSCMD_USER_RESULT usrcmd_chparser(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
#endif
