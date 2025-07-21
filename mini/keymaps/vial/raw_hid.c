
#include <stdbool.h>

#include "raw_hid.h"
#include "via.h"
#include "vial.h"

extern void raw_hid_receive_vial(uint8_t *data, uint8_t length);
extern void raw_hid_receive_qmk(uint8_t *data, uint8_t length);

#define VIAL_SUPPORT_VIA_VERSION 0x0009
#define QMK_SUPPORT_VIA_VERSION 0x000C

bool is_vial_enabled;

static bool pre_raw_hid_receive(uint8_t *msg, uint8_t len) {
    bool _continue = true;

    // Override VIA protocol version
    if (msg[0] == id_get_protocol_version) {
        const uint16_t via_version = is_vial_enabled ? VIAL_SUPPORT_VIA_VERSION : QMK_SUPPORT_VIA_VERSION;
        msg[1]                     = via_version >> 8;
        msg[2]                     = via_version & 0xFF;

        _continue = false;
    } else if (msg[0] == 0xfe) {
        switch (msg[1]) {
            case vial_get_keyboard_id: {
                is_vial_enabled = true;
                _continue = true; // vial id is set in vial.c
            } break;
        }
    }

    if (!_continue) {
        raw_hid_send(msg, len);
    }

    return _continue;
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    if (pre_raw_hid_receive(data, length)) {
        if (is_vial_enabled) {
            raw_hid_receive_vial(data, length);
        } else {
            raw_hid_receive_qmk(data, length);
        }
    }
}