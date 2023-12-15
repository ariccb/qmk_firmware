# Overrides from default rules.mk
MOUSEKEY_ENABLE = no          # Mouse keys
EXTRAKEY_ENABLE = yes         # Audio control and System control
TAP_DANCE_ENABLE = no         # Enable Tap Dance
COMBO_ENABLE = yes            # Enable Combos
BACKLIGHT_ENABLE = no	      # Backlighting
DEBOUNCE_TYPE = sym_defer_g   # Debouncing - trying to stop double space bar from being hit
CAPS_WORD_ENABLE = yes		  # enabling qmk-official caps-word feature instead of custom version
SPACE_CADET_ENABLE = no
CONSOLE_ENABLE = no

COMMAND_ENABLE = no
CONSOLE_ENABLE = no
UNICODE_ENABLE = no
SWAP_HANDS_ENABLE = no

RGBLIGHT_ENABLE = no

# To enable Rotary Encoders
ENCODER_ENABLE = yes

# To enable OLED Displays
OLED_ENABLE = yes
OLED_DRIVER = SSD1306       # OLED displays

# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no    # Breathing sleep LED during USB suspend

# Bootloader selection
BOOTLOADER = caterina       # Pro Micro

# Enable Link Time Optimization
LTO_ENABLE = yes			# Enable help with smaller firmware size

SRC += features/select_word.c
SRC += features/layer_lock.c
# If you want to change the display of OLED, you need to change here
SRC +=  ./lib/rgb_state_reader.c \
        ./burst.c \
        ./navi_logo.c \
        ./gui_state.c \
        ./fast_random.c \
        ./layer_frame.c \
        ./boot.c \
        ./draw_helper.c \
        # ./ring.c \

EXTRAFLAGS += -flto
# SRC += features/caps_word.c
# SRC += features/autocorrection.c

# Making Apple Globe (Fn) Key work
KEYBOARD_SHARED_EP = yes
