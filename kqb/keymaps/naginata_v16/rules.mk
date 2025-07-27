VIA_ENABLE = yes
VIAL_ENABLE = yes
VIAL_INSECURE = yes
UNICODE_ENABLE = yes

SRC += ../quantizer_mouse.c ../raw_hid.c
SRC += naginata_v16.c
SRC += nglist.c
SRC += nglistarray.c

# Override raw_hid_receive to support both of VIA and VIAL
$(BUILD_DIR)/obj_sekigon_keyboard_quantizer_kqb_naginata_v16/quantum/via.o:: CFLAGS += -Draw_hid_receive=raw_hid_receive_vial
SRC += tmk_core/protocol/bmp/via_qmk.c