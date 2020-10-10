#ifndef __KEY_CODES_H__
#define __KEY_CODES_H__
#ifndef _WIN32
#define VK_BACK     0x08 
#define VK_TAB      0x09 
#define VK_CLEAR    0x0C 
#define VK_RETURN   0x0D 
#define VK_SHIFT    0x10 
#define VK_CONTROL  0x11 
#define VK_MENU     0x12 
#define VK_PAUSE    0x13 
#define VK_CAPITAL  0x14 
#define VK_ESCAPE   0x1B 
#define VK_CONVERT  0x1C 
#define VK_NONCONVERT   0x1D 
#define VK_ACCEPT   0x1E 
#define VK_MODECHANGE   0x1F 
#define VK_SPACE    0x20 
#define VK_PRIOR    0x21 
#define VK_NEXT     0x22 
#define VK_END      0x23 
#define VK_HOME     0x24 
#define VK_LEFT     0x25 
#define VK_UP       0x26 
#define VK_RIGHT    0x27 
#define VK_DOWN     0x28 
#define VK_SELECT   0x29 
#define VK_PRINT    0x2A 
#define VK_EXECUTE  0x2B 
#define VK_SNAPSHOT 0x2C 
#define VK_INSERT   0x2D 
#define VK_DELETE   0x2E 
#define VK_HELP     0x2F 
#define VK_0        0x30 
#define VK_1        0x31 
#define VK_2        0x32 
#define VK_3        0x33 
#define VK_4        0x34 
#define VK_5        0x35 
#define VK_6        0x36 
#define VK_7        0x37 
#define VK_8        0x38 
#define VK_9        0x39 
//0x40 : unassigned
#define VK_A        0x41 
#define VK_B        0x42 
#define VK_C        0x43 
#define VK_D        0x44 
#define VK_E        0x45 
#define VK_F        0x46 
#define VK_G        0x47 
#define VK_H        0x48   
#define VK_I        0x49 
#define VK_J        0x4A   
#define VK_K        0x4B 
#define VK_L        0x4C   
#define VK_M        0x4D 
#define VK_N        0x4E   
#define VK_O        0x4F 
#define VK_P        0x50   
#define VK_Q        0x51 
#define VK_R        0x52   
#define VK_S        0x53 
#define VK_T        0x54   
#define VK_U        0x55 
#define VK_V        0x56   
#define VK_W        0x57 
#define VK_X        0x58   
#define VK_Y        0x59 
#define VK_Z        0x5A   
#define VK_LWIN     0x5B 
#define VK_RWIN     0x5C   
#define VK_APPS     0x5D 
#define VK_SLEEP    0x5F   
#define VK_NUMPAD0  0x60 
#define VK_NUMPAD1  0x61   
#define VK_NUMPAD2  0x62 
#define VK_NUMPAD3  0x63   
#define VK_NUMPAD4  0x64 
#define VK_NUMPAD5  0x65   
#define VK_NUMPAD6  0x66 
#define VK_NUMPAD7  0x67   
#define VK_NUMPAD8  0x68 
#define VK_NUMPAD9  0x69   
#define VK_MULTIPLY 0x6A 
#define VK_ADD      0x6B   
#define VK_SEPARATOR    0x6C 
#define VK_SUBTRACT 0x6D   
#define VK_DECIMAL  0x6E 
#define VK_DIVIDE   0x6F   
#define VK_F1       0x70 
#define VK_F2       0x71   
#define VK_F3       0x72 
#define VK_F4       0x73   
#define VK_F5       0x74 
#define VK_F6       0x75   
#define VK_F7       0x76 
#define VK_F8       0x77   
#define VK_F9       0x78 
#define VK_F10      0x79   
#define VK_F11      0x7A 
#define VK_F12      0x7B   
#define VK_F13      0x7C 
#define VK_F14      0x7D   
#define VK_F15      0x7E 
#define VK_F16      0x7F   
#define VK_F17      0x80 
#define VK_F18      0x81   
#define VK_F19      0x82 
#define VK_F20      0x83   
#define VK_F21      0x84 
#define VK_F22      0x85   
#define VK_F23      0x86 
#define VK_F24      0x87   
#define VK_NUMLOCK  0x90 
#define VK_SCROLL   0x91   
#define VK_OEM_NEC_EQUAL    0x92 

//VK_L* & VK_R* - left and right Alt 
//Ctrl and Shift virtual keys.
//Used only as parameters to GetAsyncKeyState() and GetKeyState().
//No other API or message will distinguish left and right keys in this way.
#define VK_LSHIFT   0xA0 
#define VK_RSHIFT   0xA1   
#define VK_LCONTROL 0xA2 
#define VK_RCONTROL 0xA3 
#define VK_LMENU    0xA4 
#define VK_RMENU    0xA5   
#define VK_VOLUME_MUTE  0xAD   
#define VK_VOLUME_DOWN  0xAE   
#define VK_VOLUME_UP    0xAF   
#define VK_OEM_1    0xBA 
// ';:' for US   
#define VK_OEM_PLUS 0xBB 
// '+' any country 
#define VK_OEM_COMMA    0xBC 
// ',' any country   
#define VK_OEM_MINUS    0xBD 
// '-' any country   
#define VK_OEM_PERIOD   0xBE 
// '.' any country   
#define VK_OEM_2    0xBF 
// '/?' for US   
#define VK_OEM_3    0xC0 
#define VK_OEM_4    0xDB 
//  '[{' for US   
#define VK_OEM_5    0xDC 
//  '\|' for US   
#define VK_OEM_6    0xDD 
//  ']}' for US   
#define VK_OEM_7    0xDE 
//  ''"' for US   
#define VK_OEM_8    0xDF

/* Translate keycodes to win32 defined keycodes. */
uint16_t keycode_table[UINT16_MAX] = {0};
#endif /* _WIN32 */

#ifdef _GTK
#include <gdk/gdkkeysyms.h>
void init_keycode_table()
{
    keycode_table[GDK_KEY_BackSpace] = VK_BACK;
    keycode_table[GDK_KEY_Tab] = VK_TAB;
    keycode_table[GDK_KEY_Clear] = VK_CLEAR;
    keycode_table[GDK_KEY_Return] = VK_RETURN;
    keycode_table[GDK_KEY_Pause] = VK_PAUSE;
    keycode_table[GDK_KEY_Scroll_Lock] = VK_SCROLL;
    keycode_table[GDK_KEY_Escape] = VK_ESCAPE;
    keycode_table[GDK_KEY_Delete] = VK_DELETE;
    keycode_table[GDK_KEY_Home] = VK_HOME;
    keycode_table[GDK_KEY_Left] = VK_LEFT;
    keycode_table[GDK_KEY_Up] = VK_UP;
    keycode_table[GDK_KEY_Right] = VK_RIGHT;
    keycode_table[GDK_KEY_Down] = VK_DOWN;
    keycode_table[GDK_KEY_Prior] = VK_PRIOR;
    keycode_table[GDK_KEY_Page_Up] = VK_PRIOR;
    keycode_table[GDK_KEY_Next] = VK_NEXT;
    keycode_table[GDK_KEY_Page_Down] = VK_NEXT;
    keycode_table[GDK_KEY_End] = VK_END;
    keycode_table[GDK_KEY_Select] = VK_SELECT;
    keycode_table[GDK_KEY_Print] = VK_PRINT;
    keycode_table[GDK_KEY_Execute] = VK_EXECUTE;
    keycode_table[GDK_KEY_Insert] = VK_INSERT;
    keycode_table[GDK_KEY_F1] = VK_F1;
    keycode_table[GDK_KEY_F2] = VK_F2;
    keycode_table[GDK_KEY_F3] = VK_F3;
    keycode_table[GDK_KEY_F4] = VK_F4;
    keycode_table[GDK_KEY_F5] = VK_F5;
    keycode_table[GDK_KEY_F6] = VK_F6;
    keycode_table[GDK_KEY_F7] = VK_F7;
    keycode_table[GDK_KEY_F8] = VK_F8;
    keycode_table[GDK_KEY_F9] = VK_F9;
    keycode_table[GDK_KEY_F10] = VK_F10;
    keycode_table[GDK_KEY_F11] = VK_F11;
    keycode_table[GDK_KEY_F12] = VK_F12;
    // keycode_table[GDK_KEY_Shift_L] = VK_SHIFT;
    // keycode_table[GDK_KEY_Shift_R] = VK_SHIFT;
    keycode_table[GDK_KEY_Shift_L] = VK_LSHIFT;
    keycode_table[GDK_KEY_Shift_R] = VK_RSHIFT;
    // keycode_table[GDK_KEY_Control_L] = VK_CONTROL;
    // keycode_table[GDK_KEY_Control_R] = VK_CONTROL;
    keycode_table[GDK_KEY_Control_L] = VK_LCONTROL;
    keycode_table[GDK_KEY_Control_R] = VK_RCONTROL;
    keycode_table[GDK_KEY_Caps_Lock] = VK_CAPITAL;
    keycode_table[GDK_KEY_Meta_L] = VK_LWIN;
    keycode_table[GDK_KEY_Meta_R] = VK_RWIN;
    keycode_table[GDK_KEY_Alt_L] = VK_LMENU;
    keycode_table[GDK_KEY_Alt_R] = VK_RMENU;
    keycode_table[GDK_KEY_space] = VK_SPACE;
    keycode_table[GDK_KEY_0] = VK_0;
    keycode_table[GDK_KEY_1] = VK_1;
    keycode_table[GDK_KEY_2] = VK_2;
    keycode_table[GDK_KEY_3] = VK_3;
    keycode_table[GDK_KEY_4] = VK_4;
    keycode_table[GDK_KEY_5] = VK_5;
    keycode_table[GDK_KEY_6] = VK_6;
    keycode_table[GDK_KEY_7] = VK_7;
    keycode_table[GDK_KEY_8] = VK_8;
    keycode_table[GDK_KEY_9] = VK_9;
    keycode_table[GDK_KEY_a] = VK_A;
    keycode_table[GDK_KEY_b] = VK_B;
    keycode_table[GDK_KEY_c] = VK_C;
    keycode_table[GDK_KEY_d] = VK_D;
    keycode_table[GDK_KEY_e] = VK_E;
    keycode_table[GDK_KEY_f] = VK_F;
    keycode_table[GDK_KEY_g] = VK_G;
    keycode_table[GDK_KEY_h] = VK_H;
    keycode_table[GDK_KEY_i] = VK_I;
    keycode_table[GDK_KEY_j] = VK_J;
    keycode_table[GDK_KEY_k] = VK_K;
    keycode_table[GDK_KEY_l] = VK_L;
    keycode_table[GDK_KEY_m] = VK_M;
    keycode_table[GDK_KEY_n] = VK_N;
    keycode_table[GDK_KEY_o] = VK_O;
    keycode_table[GDK_KEY_p] = VK_P;
    keycode_table[GDK_KEY_q] = VK_Q;
    keycode_table[GDK_KEY_r] = VK_R;
    keycode_table[GDK_KEY_s] = VK_S;
    keycode_table[GDK_KEY_t] = VK_T;
    keycode_table[GDK_KEY_u] = VK_U;
    keycode_table[GDK_KEY_v] = VK_V;
    keycode_table[GDK_KEY_w] = VK_W;
    keycode_table[GDK_KEY_x] = VK_X;
    keycode_table[GDK_KEY_y] = VK_Y;
    keycode_table[GDK_KEY_z] = VK_Z;
    keycode_table[GDK_KEY_A] = VK_A;
    keycode_table[GDK_KEY_B] = VK_B;
    keycode_table[GDK_KEY_C] = VK_C;
    keycode_table[GDK_KEY_D] = VK_D;
    keycode_table[GDK_KEY_E] = VK_E;
    keycode_table[GDK_KEY_F] = VK_F;
    keycode_table[GDK_KEY_G] = VK_G;
    keycode_table[GDK_KEY_H] = VK_H;
    keycode_table[GDK_KEY_I] = VK_I;
    keycode_table[GDK_KEY_J] = VK_J;
    keycode_table[GDK_KEY_K] = VK_K;
    keycode_table[GDK_KEY_L] = VK_L;
    keycode_table[GDK_KEY_M] = VK_M;
    keycode_table[GDK_KEY_N] = VK_N;
    keycode_table[GDK_KEY_O] = VK_O;
    keycode_table[GDK_KEY_P] = VK_P;
    keycode_table[GDK_KEY_Q] = VK_Q;
    keycode_table[GDK_KEY_R] = VK_R;
    keycode_table[GDK_KEY_S] = VK_S;
    keycode_table[GDK_KEY_T] = VK_T;
    keycode_table[GDK_KEY_U] = VK_U;
    keycode_table[GDK_KEY_V] = VK_V;
    keycode_table[GDK_KEY_W] = VK_W;
    keycode_table[GDK_KEY_X] = VK_X;
    keycode_table[GDK_KEY_Y] = VK_Y;
    keycode_table[GDK_KEY_Z] = VK_Z;
}
#endif /* _GTK */

#endif /* __KEY_CODES_H__ */