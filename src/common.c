#include "common.h"

void *canopy_calloc(size_t count, size_t size)
{
    return calloc(count, size);
}
void canopy_free(void *ptr)
{
    free(ptr);
}
void *canopy_malloc(size_t size)
{
    return malloc(size);
}
void *canopy_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

const char* canopy_key_to_string(keys key)
{
    switch (key) {
        // Alphabet
        case CANOPY_KEY_A: return "a";
        case CANOPY_KEY_B: return "b";
        case CANOPY_KEY_C: return "c";
        case CANOPY_KEY_D: return "d";
        case CANOPY_KEY_E: return "e";
        case CANOPY_KEY_F: return "f";
        case CANOPY_KEY_G: return "g";
        case CANOPY_KEY_H: return "h";
        case CANOPY_KEY_I: return "i";
        case CANOPY_KEY_J: return "j";
        case CANOPY_KEY_K: return "k";
        case CANOPY_KEY_L: return "l";
        case CANOPY_KEY_M: return "m";
        case CANOPY_KEY_N: return "n";
        case CANOPY_KEY_O: return "o";
        case CANOPY_KEY_P: return "p";
        case CANOPY_KEY_Q: return "q";
        case CANOPY_KEY_R: return "r";
        case CANOPY_KEY_S: return "s";
        case CANOPY_KEY_T: return "t";
        case CANOPY_KEY_U: return "u";
        case CANOPY_KEY_V: return "v";
        case CANOPY_KEY_W: return "w";
        case CANOPY_KEY_X: return "x";
        case CANOPY_KEY_Y: return "y";
        case CANOPY_KEY_Z: return "z";
        case CANOPY_KEY_AE: return "æ";
        case CANOPY_KEY_OE: return "ø";
        case CANOPY_KEY_AA: return "å";

        // Number row
        case CANOPY_KEY_0: return "0";
        case CANOPY_KEY_1: return "1";
        case CANOPY_KEY_2: return "2";
        case CANOPY_KEY_3: return "3";
        case CANOPY_KEY_4: return "4";
        case CANOPY_KEY_5: return "5";
        case CANOPY_KEY_6: return "6";
        case CANOPY_KEY_7: return "7";
        case CANOPY_KEY_8: return "8";
        case CANOPY_KEY_9: return "9";

        // Numpad
        case CANOPY_KEY_NUMPAD0: return "numpad 0";
        case CANOPY_KEY_NUMPAD1: return "numpad 1";
        case CANOPY_KEY_NUMPAD2: return "numpad 2";
        case CANOPY_KEY_NUMPAD3: return "numpad 3";
        case CANOPY_KEY_NUMPAD4: return "numpad 4";
        case CANOPY_KEY_NUMPAD5: return "numpad 5";
        case CANOPY_KEY_NUMPAD6: return "numpad 6";
        case CANOPY_KEY_NUMPAD7: return "numpad 7";
        case CANOPY_KEY_NUMPAD8: return "numpad 8";
        case CANOPY_KEY_NUMPAD9: return "numpad 9";

        // Function keys
        case CANOPY_KEY_F1:  return "f1";
        case CANOPY_KEY_F2:  return "f2";
        case CANOPY_KEY_F3:  return "f3";
        case CANOPY_KEY_F4:  return "f4";
        case CANOPY_KEY_F5:  return "f5";
        case CANOPY_KEY_F6:  return "f6";
        case CANOPY_KEY_F7:  return "f7";
        case CANOPY_KEY_F8:  return "f8";
        case CANOPY_KEY_F9:  return "f9";
        case CANOPY_KEY_F10: return "f10";
        case CANOPY_KEY_F11: return "f11";
        case CANOPY_KEY_F12: return "f12";
        case CANOPY_KEY_F13: return "f13";
        case CANOPY_KEY_F14: return "f14";
        case CANOPY_KEY_F15: return "f15";
        case CANOPY_KEY_F16: return "f16";
        case CANOPY_KEY_F17: return "f17";
        case CANOPY_KEY_F18: return "f18";
        case CANOPY_KEY_F19: return "f19";
        case CANOPY_KEY_F20: return "f20";

        // Control & navigation
        case CANOPY_KEY_ESCAPE: return "escape";
        case CANOPY_KEY_SPACE: return "space";
        case CANOPY_KEY_TAB: return "tab";
        case CANOPY_KEY_RETURN: return "return";
        case CANOPY_KEY_ENTER: return "enter";
        case CANOPY_KEY_DELETE: return "delete";
        case CANOPY_KEY_BACKSPACE: return "backspace";

        case CANOPY_KEY_UP: return "up";
        case CANOPY_KEY_DOWN: return "down";
        case CANOPY_KEY_LEFT: return "left";
        case CANOPY_KEY_RIGHT: return "right";

        case CANOPY_KEY_PAGE_UP: return "page up";
        case CANOPY_KEY_PAGE_DOWN: return "page down";
        case CANOPY_KEY_PAGE_HOME: return "home";
        case CANOPY_KEY_PAGE_END: return "end";

        case CANOPY_KEY_INSERT: return "insert";

        // Modifiers
        case CANOPY_KEY_LSHIFT: return "l_shift";
        case CANOPY_KEY_RSHIFT: return "r_shift";
        case CANOPY_KEY_LCONTROL: return "l_control";
        case CANOPY_KEY_RCONTROL: return "r_control";
        case CANOPY_KEY_LOPTION: return "l_option";
        case CANOPY_KEY_ROPTION: return "r_option";
        case CANOPY_KEY_LCOMMAND: return "l_command";
        case CANOPY_KEY_RCOMMAND: return "r_command";
        case CANOPY_KEY_CAPS_LOCK: return "caps lock";
        case CANOPY_KEY_FN: return "fn";

        // Symbols
//        case CANOPY_KEY_APOSTROPHE: return "'"; // Conflicting with æ
        case CANOPY_KEY_BACKSLASH: return "\\";
        case CANOPY_KEY_COMMA: return ",";
        case CANOPY_KEY_EQUAL: return "=";
        case CANOPY_KEY_GRAVE: return "`";
//        case CANOPY_KEY_LBRACKET: return "["; // Conflicting with å
        case CANOPY_KEY_MINUS: return "-";
        case CANOPY_KEY_PERIOD: return ".";
        case CANOPY_KEY_RBRACKET: return "]";
//        case CANOPY_KEY_SEMICOLON: return ";"; // Conflicing with ø
        case CANOPY_KEY_SLASH: return "/";

        // Numpad operations
        case CANOPY_KEY_ADD: return "numpad +";
        case CANOPY_KEY_SUBTRACT: return "numpad -";
        case CANOPY_KEY_MULTIPLY: return "numpad *";
        case CANOPY_KEY_DIVIDE: return "numpad /";
        case CANOPY_KEY_DECIMAL: return "numpad .";
        case CANOPY_KEY_NUMPAD_EQUAL: return "numpad =";

        // Unknowns or edge keys
        case CANOPY_KEY_AX_KEYS: return "ax";
        case CANOPY_KEY_UMLOCK: return "num lock";

        default: return "undefined key";
    }
}

