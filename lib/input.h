#ifndef INPUT_H
#define INPUT_H

typedef enum keys {
        CANOPY_KEY_A = 0x00,
        CANOPY_KEY_B = 0x0B,
        CANOPY_KEY_C = 0x08,
        CANOPY_KEY_D = 0x02,
        CANOPY_KEY_E = 0x0E,
        CANOPY_KEY_F = 0x03,
        CANOPY_KEY_G = 0x05,
        CANOPY_KEY_H = 0x04,
        CANOPY_KEY_I = 0x22,
        CANOPY_KEY_J = 0x26,
        CANOPY_KEY_K = 0x28,
        CANOPY_KEY_L = 0x25,
        CANOPY_KEY_M = 0x2E,
        CANOPY_KEY_N = 0x2D,
        CANOPY_KEY_O = 0x1F,
        CANOPY_KEY_P = 0x23,
        CANOPY_KEY_Q = 0x0C,
        CANOPY_KEY_R = 0x0F,
        CANOPY_KEY_S = 0x01,
        CANOPY_KEY_T = 0x11,
        CANOPY_KEY_U = 0x20,
        CANOPY_KEY_V = 0x09,
        CANOPY_KEY_W = 0x0D,
        CANOPY_KEY_X = 0x07,
        CANOPY_KEY_Y = 0x10,
        CANOPY_KEY_Z = 0x06,
        CANOPY_KEY_AE = 0x27,
        CANOPY_KEY_OE = 0x29,
        CANOPY_KEY_AA = 0x21,

        CANOPY_KEY_NUMPAD0 = 0x52,
        CANOPY_KEY_NUMPAD1 = 0x53,
        CANOPY_KEY_NUMPAD2 = 0x54,
        CANOPY_KEY_NUMPAD3 = 0x55,
        CANOPY_KEY_NUMPAD4 = 0x56,
        CANOPY_KEY_NUMPAD5 = 0x57,
        CANOPY_KEY_NUMPAD6 = 0x58,
        CANOPY_KEY_NUMPAD7 = 0x59,
        CANOPY_KEY_NUMPAD8 = 0x5B,
        CANOPY_KEY_NUMPAD9 = 0x5C,

        CANOPY_KEY_1 = 0x12,
        CANOPY_KEY_2 = 0x13,
        CANOPY_KEY_3 = 0x14,
        CANOPY_KEY_4 = 0x15,
        CANOPY_KEY_5 = 0x17,
        CANOPY_KEY_6 = 0x16,
        CANOPY_KEY_7 = 0x1A,
        CANOPY_KEY_8 = 0x1C,
        CANOPY_KEY_9 = 0x19,
        CANOPY_KEY_0 = 0x1D,

        CANOPY_KEY_F1 = 0x7A,
        CANOPY_KEY_F2 = 0x78,
        CANOPY_KEY_F3 = 0x63,
        CANOPY_KEY_F4 = 0x76,
        CANOPY_KEY_F5 = 0x60,
        CANOPY_KEY_F6 = 0x61,
        CANOPY_KEY_F7 = 0x62,
        CANOPY_KEY_F8 = 0x64,
        CANOPY_KEY_F9 = 0x65,
        CANOPY_KEY_F10 = 0x6D,
        CANOPY_KEY_F11 = 0x67,
        CANOPY_KEY_F12 = 0x6F,
        CANOPY_KEY_F13 = 0x69, // PRINT
        CANOPY_KEY_F14 = 0x6B,
        CANOPY_KEY_F15 = 0x71,
        CANOPY_KEY_F16 = 0x6A,
        CANOPY_KEY_F17 = 0x40,
        CANOPY_KEY_F18 = 0x4F,
        CANOPY_KEY_F19 = 0x50,
        CANOPY_KEY_F20 = 0x5A,

        CANOPY_KEY_ESCAPE = 0x35,
        CANOPY_KEY_SPACE = 0x31,
        CANOPY_KEY_TAB = 0x30,
        CANOPY_KEY_RETURN = 0x24,
        CANOPY_KEY_ENTER = 0x4C,
        CANOPY_KEY_DELETE = 0x75,

        CANOPY_KEY_UP = 0x7E,
        CANOPY_KEY_DOWN = 0x7D,
        CANOPY_KEY_LEFT =0x7B,
        CANOPY_KEY_RIGHT = 0x7C,

        CANOPY_KEY_PAGE_UP = 0x74,
        CANOPY_KEY_PAGE_DOWN = 0x79,
        CANOPY_KEY_PAGE_HOME = 0x73,
        CANOPY_KEY_PAGE_END = 0x77,

        CANOPY_KEY_LSHIFT = 0x38,
        CANOPY_KEY_RSHIFT = 0x3C,

        CANOPY_KEY_LCONTROL = 0x3B,
        CANOPY_KEY_RCONTROL = 0x3E,

        CANOPY_KEY_LOPTION = 0x3A, // alt/option
        CANOPY_KEY_ROPTION = 0x3D, // alt/option

        CANOPY_KEY_LCOMMAND = 0x37,
        CANOPY_KEY_RCOMMAND = 0x36,

        CANOPY_KEY_CAPS_LOCK = 0x39,
        CANOPY_KEY_FN = 0x3F,

        CANOPY_KEY_APOSTROPHE = 0x27,
        CANOPY_KEY_BACKSLASH = 0x2A,
        CANOPY_KEY_COMMA = 0x2B,
        CANOPY_KEY_EQUAL = 0x18,
        CANOPY_KEY_GRAVE = 0x32,
        CANOPY_KEY_LBRACKET = 0x21,
        CANOPY_KEY_MINUS = 0x1B,
        CANOPY_KEY_PERIOD = 0x2F,
        CANOPY_KEY_RBRACKET = 0x1E,
        CANOPY_KEY_SEMICOLON = 0x29,
        CANOPY_KEY_SLASH = 0x2C,
        CANOPY_KEY_MAX_KEYS = 0x0A,
        CANOPY_KEY_BACKSPACE = 0x33,
        CANOPY_KEY_INSERT =  0x72,
        CANOPY_KEY_AX_KEYS = 0x6E,
        CANOPY_KEY_UMLOCK = 0x47,

        CANOPY_KEY_ADD = 0x45,
        CANOPY_KEY_DECIMAL = 0x41,
        CANOPY_KEY_DIVIDE = 0x4B,
        CANOPY_KEY_NUMPAD_EQUAL = 0x51,
        CANOPY_KEY_MULTIPLY = 0x43,
        CANOPY_KEY_SUBTRACT = 0x4E,
}keys;

typedef enum {
        CANOPY_MOUSE_BUTTON_LEFT,
        CANOPY_MOUSE_BUTTON_RIGHT,
        CANOPY_MOUSE_BUTTON_MIDDLE,
        CANOPY_MAX_MOUSE_BUTTONS
}mouse_buttons;

#endif // INPUT_H
