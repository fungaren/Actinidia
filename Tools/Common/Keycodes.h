#ifndef __KEY_CODES_H__
#define __KEY_CODES_H__

#ifdef _WIN32

#endif /* _WIN32 */
#ifdef _GTK
#include <gdk/gdkkeysyms.h>

#define KEY_BackSpace   GDK_KEY_BackSpace
#define KEY_Tab         GDK_KEY_Tab
#define KEY_Linefeed    GDK_KEY_Linefeed
#define KEY_Clear       GDK_KEY_Clear
#define KEY_Return      GDK_KEY_Return
#define KEY_Pause       GDK_KEY_Pause
#define KEY_Scroll_Lock GDK_KEY_Scroll_Lock
#define KEY_Sys_Req     GDK_KEY_Sys_Req
#define KEY_Escape      GDK_KEY_Escape
#define KEY_Delete      GDK_KEY_Delete

#define KEY_Home        GDK_KEY_Home
#define KEY_Left        GDK_KEY_Left
#define KEY_Up          GDK_KEY_Up
#define KEY_Right       GDK_KEY_Right
#define KEY_Down        GDK_KEY_Down
#define KEY_Prior       GDK_KEY_Prior
#define KEY_Page_Up     GDK_KEY_Page_Up
#define KEY_Next        GDK_KEY_Next
#define KEY_Page_Down   GDK_KEY_Page_Down
#define KEY_End         GDK_KEY_End
#define KEY_Begin       GDK_KEY_Begin
#define KEY_Select      GDK_KEY_Select
#define KEY_Print       GDK_KEY_Print
#define KEY_Execute     GDK_KEY_Execute
#define KEY_Insert      GDK_KEY_Insert

#define KEY_F1          GDK_KEY_F1
#define KEY_F2          GDK_KEY_F2
#define KEY_F3          GDK_KEY_F3
#define KEY_F4          GDK_KEY_F4
#define KEY_F5          GDK_KEY_F5
#define KEY_F6          GDK_KEY_F6
#define KEY_F7          GDK_KEY_F7
#define KEY_F8          GDK_KEY_F8
#define KEY_F9          GDK_KEY_F9
#define KEY_F10         GDK_KEY_F10
#define KEY_F11         GDK_KEY_F11
#define KEY_F12         GDK_KEY_F12

#define KEY_Shift_L     GDK_KEY_Shift_L
#define KEY_Shift_R     GDK_KEY_Shift_R
#define KEY_Control_L   GDK_KEY_Control_L
#define KEY_Control_R   GDK_KEY_Control_R
#define KEY_Caps_Lock   GDK_KEY_Caps_Lock
#define KEY_Shift_Lock  GDK_KEY_Shift_Lock
#define KEY_Meta_L      GDK_KEY_Meta_L
#define KEY_Meta_R      GDK_KEY_Meta_R
#define KEY_Alt_L       GDK_KEY_Alt_L
#define KEY_Alt_R       GDK_KEY_Alt_R
#define KEY_Super_L     GDK_KEY_Super_L
#define KEY_Super_R     GDK_KEY_Super_R
#define KEY_Hyper_L     GDK_KEY_Hyper_L
#define KEY_Hyper_R     GDK_KEY_Hyper_R

#define KEY_space       GDK_KEY_space
#define KEY_exclam      GDK_KEY_exclam
#define KEY_quotedbl    GDK_KEY_quotedbl
#define KEY_numbersign  GDK_KEY_numbersign
#define KEY_dollar      GDK_KEY_dollar
#define KEY_percent     GDK_KEY_percent
#define KEY_ampersand   GDK_KEY_ampersand
#define KEY_apostrophe  GDK_KEY_apostrophe
#define KEY_quoteright  GDK_KEY_quoteright
#define KEY_parenleft   GDK_KEY_parenleft
#define KEY_parenright  GDK_KEY_parenright
#define KEY_asterisk    GDK_KEY_asterisk
#define KEY_plus        GDK_KEY_plus
#define KEY_comma       GDK_KEY_comma
#define KEY_minus       GDK_KEY_minus
#define KEY_period      GDK_KEY_period
#define KEY_slash       GDK_KEY_slash
#define KEY_0           GDK_KEY_0
#define KEY_1           GDK_KEY_1
#define KEY_2           GDK_KEY_2
#define KEY_3           GDK_KEY_3
#define KEY_4           GDK_KEY_4
#define KEY_5           GDK_KEY_5
#define KEY_6           GDK_KEY_6
#define KEY_7           GDK_KEY_7
#define KEY_8           GDK_KEY_8
#define KEY_9           GDK_KEY_9
#define KEY_colon       GDK_KEY_colon
#define KEY_semicolon   GDK_KEY_semicolon
#define KEY_less        GDK_KEY_less
#define KEY_equal       GDK_KEY_equal
#define KEY_greater     GDK_KEY_greater
#define KEY_question    GDK_KEY_question
#define KEY_at          GDK_KEY_at
#define KEY_A           GDK_KEY_A
#define KEY_B           GDK_KEY_B
#define KEY_C           GDK_KEY_C
#define KEY_D           GDK_KEY_D
#define KEY_E           GDK_KEY_E
#define KEY_F           GDK_KEY_F
#define KEY_G           GDK_KEY_G
#define KEY_H           GDK_KEY_H
#define KEY_I           GDK_KEY_I
#define KEY_J           GDK_KEY_J
#define KEY_K           GDK_KEY_K
#define KEY_L           GDK_KEY_L
#define KEY_M           GDK_KEY_M
#define KEY_N           GDK_KEY_N
#define KEY_O           GDK_KEY_O
#define KEY_P           GDK_KEY_P
#define KEY_Q           GDK_KEY_Q
#define KEY_R           GDK_KEY_R
#define KEY_S           GDK_KEY_S
#define KEY_T           GDK_KEY_T
#define KEY_U           GDK_KEY_U
#define KEY_V           GDK_KEY_V
#define KEY_W           GDK_KEY_W
#define KEY_X           GDK_KEY_X
#define KEY_Y           GDK_KEY_Y
#define KEY_Z           GDK_KEY_Z
#define KEY_bracketleft GDK_KEY_bracketleft
#define KEY_backslash   GDK_KEY_backslash
#define KEY_bracketright    GDK_KEY_bracketright
#define KEY_asciicircum GDK_KEY_asciicircum
#define KEY_underscore  GDK_KEY_underscore
#define KEY_grave       GDK_KEY_grave
#define KEY_quoteleft   GDK_KEY_quoteleft
#define KEY_a           GDK_KEY_a
#define KEY_b           GDK_KEY_b
#define KEY_c           GDK_KEY_c
#define KEY_d           GDK_KEY_d
#define KEY_e           GDK_KEY_e
#define KEY_f           GDK_KEY_f
#define KEY_g           GDK_KEY_g
#define KEY_h           GDK_KEY_h
#define KEY_i           GDK_KEY_i
#define KEY_j           GDK_KEY_j
#define KEY_k           GDK_KEY_k
#define KEY_l           GDK_KEY_l
#define KEY_m           GDK_KEY_m
#define KEY_n           GDK_KEY_n
#define KEY_o           GDK_KEY_o
#define KEY_p           GDK_KEY_p
#define KEY_q           GDK_KEY_q
#define KEY_r           GDK_KEY_r
#define KEY_s           GDK_KEY_s
#define KEY_t           GDK_KEY_t
#define KEY_u           GDK_KEY_u
#define KEY_v           GDK_KEY_v
#define KEY_w           GDK_KEY_w
#define KEY_x           GDK_KEY_x
#define KEY_y           GDK_KEY_y
#define KEY_z           GDK_KEY_z

#endif /* _GTK */

#endif /* __KEY_CODES_H__ */