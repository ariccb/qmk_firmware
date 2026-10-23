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

# Adafruit KB2040 in the Pro Micro sockets. Omit CONVERT_TO for the Pro Micro.
# qmk compile -kb clickety_split/leeloo -km ariccb_totem -e CONVERT_TO=kb2040
ifeq ($(strip $(CONVERT_TO)), kb2040)
    SERIAL_DRIVER = vendor
    OLED_ENABLE = yes
    OLED_DRIVER = SSD1306
endif
