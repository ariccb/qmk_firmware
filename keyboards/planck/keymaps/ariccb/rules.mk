# Build Options
#   change to "no" to disable the options, or define them in the Makefile in 
#   the appropriate keymap folder that will get included automatically
#
SRC += features/select_word.c
SRC += features/caps_word.c

BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = yes        # Commands for debug and configuration
NKRO_ENABLE = yes           # Nkey Rollover - if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
BACKLIGHT_ENABLE = yes      # Enable keyboard backlight functionality
MIDI_ENABLE = yes           # MIDI controls
AUDIO_ENABLE = yes          # Audio output
RGBLIGHT_ENABLE = yes       # Enable WS2812 RGB underlight. 
TAP_DANCE_ENABLE = yes 		# Enable Tap Dance Features
LTO_ENABLE = no 			# Link Time Optimization
DIP_SWITCH_ENABLE=no  		# Recommended to fix no startup sound issue
COMBO_ENABLE = yes			# Enable Combos


# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no    	# Breathing sleep LED during USB suspend



