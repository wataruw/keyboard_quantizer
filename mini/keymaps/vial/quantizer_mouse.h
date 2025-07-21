// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

extern uint8_t  encoder_modifier;
extern uint16_t encoder_modifier_pressed_ms;
extern bool     is_encoder_action;

void post_process_record_mouse(uint16_t keycode, keyrecord_t* record);
bool process_record_mouse(uint16_t keycode, keyrecord_t* record);
void set_mouse_gesture_threshold(uint16_t val);
bool pre_process_record_mouse(uint16_t keycode, keyrecord_t *record);