/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keymap_common.h"
#include "report.h"
#include "keycode.h"
#include "action_layer.h"
#include "action.h"
#include "action_macro.h"
#include "debug.h"
#include "backlight.h"
#include "keymap_midi.h"

/* converts key to action */
action_t action_for_key(uint8_t layer, keypos_t key)
{
    // 16bit keycodes - important
    keycode_t keycode = keymap_key_to_keycode(layer, key);

    if (keycode >= 0x0100 && keycode < 0x2000) {
        // Has a modifier
        action_t action;
        // Split it up
        action.code = ACTION_MODS_KEY(keycode >> 8, keycode & 0xFF);
        return action;
    } else if (keycode >= 0x2000 && keycode < 0x3000) {
        // Is a shortcut for function layer, pull last 12bits
        return keymap_func_to_action(keycode & 0xFFF);
    } else if (keycode >= 0x3000 && keycode < 0x4000) {
        action_t action;
        action.code = ACTION_MACRO(keycode & 0xFF);
        return action;
    } else if (keycode >= BL_0 && keycode <= BL_15) {
        action_t action;
        action.code = ACTION_BACKLIGHT_LEVEL(keycode & 0x000F);
        return action;
    } else if (keycode == BL_DEC) {
        action_t action;
        action.code = ACTION_BACKLIGHT_DECREASE();
        return action;
    } else if (keycode == BL_INC) {
        action_t action;
        action.code = ACTION_BACKLIGHT_INCREASE();
        return action;
    } else if (keycode == BL_TOGG) {
        action_t action;
        action.code = ACTION_BACKLIGHT_TOGGLE();
        return action;
    } else if (keycode == BL_STEP) {
        action_t action;
        action.code = ACTION_BACKLIGHT_STEP();
        return action;
    } else if (keycode == RESET) {
        bootloader_jump();
        return;
    } else if (keycode == DEBUG) {
        print("\nDEBUG: enabled.\n");
        debug_enable = true;
        return;
    } else if (keycode >= 0x6000 && keycode < 0x7000) {
        action_t action;
        action.code =  ACTION_FUNCTION_OPT(keycode & 0xFF, (keycode & 0x0F00) >> 8);
        return action;
    } else if (keycode >= 0x8000) {
        action_t action;
        uint16_t unicode = keycode & ~(0x8000);
        action.code =  ACTION_FUNCTION_OPT(unicode & 0xFF, (unicode & 0xFF00) >> 8);
        return action;
    }

    return action_for_key_default(layer, key);

}

/* translates key to keycode */
uint16_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    // Read entire word (16bits)
    return pgm_read_word(&keymaps[(layer)][(key.row)][(key.col)]);
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(keycode_t keycode)
{
    return (action_t){ .code = pgm_read_word(&fn_actions[FN_INDEX(keycode)]) };
}

action_t keymap_func_to_action(keycode_t keycode)
{
    // For FUNC without 8bit limit
    return (action_t){ .code = pgm_read_word(&fn_actions[(int)keycode]) };
}
