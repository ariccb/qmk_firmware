MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
COMBO_ENABLE = yes
CAPS_WORD_ENABLE = yes
TAP_DANCE_ENABLE = no
SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no
MAGIC_ENABLE = yes
NKRO_ENABLE = no
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
BOOTMAGIC_ENABLE = no
RGBLIGHT_ENABLE = no
BACKLIGHT_ENABLE = no
OLED_ENABLE = no
ENCODER_ENABLE = yes
BOOTLOADER = caterina
LTO_ENABLE = yes

SRC += features/adaptive_keys.c
SRC += features/hold_tap.c
SRC += chatter_guard.c

# Adafruit KB2040 in the Pro Micro sockets. Omit CONVERT_TO for the Pro Micro.
# qmk compile -kb clickety_split/leeloo -km ariccb_totem -e CONVERT_TO=kb2040
ifeq ($(strip $(CONVERT_TO)), kb2040)
    SERIAL_DRIVER = vendor
    OLED_ENABLE = yes
    OLED_DRIVER = SSD1306
    # HELL0 NAVI (druotoni). navi_font.c is pulled in by OLED_FONT_H.
    SRC += navi/boot.c
    SRC += navi/burst.c
    SRC += navi/draw_helper.c
    SRC += navi/fast_random.c
    SRC += navi/gui_state.c
    SRC += navi/layer_frame.c
    SRC += navi/navi_logo.c
    SRC += navi/ring.c
    SRC += navi/oled_navi.c
endif
