#ifndef __KEY_CODES_H__
#define __KEY_CODES_H__

#ifdef _WIN32
#include <WinUser.h>
#define KEY_BackSpace   VK_BACK
#define KEY_Tab         VK_TAB
#define KEY_Linefeed    0
#define KEY_Clear       VK_CLEAR
#define KEY_Return      VK_RETURN
#define KEY_Pause       VK_PAUSE
#define KEY_Scroll_Lock VK_SCROLL
#define KEY_Sys_Req     0
#define KEY_Escape      VK_ESCAPE
#define KEY_Delete      VK_DELETE

#define KEY_Home        VK_HOME
#define KEY_Left        VK_LEFT
#define KEY_Up          VK_UP
#define KEY_Right       VK_RIGHT
#define KEY_Down        VK_DOWN
#define KEY_Prior       VK_PRIOR
#define KEY_Page_Up     VK_PRIOR
#define KEY_Next        VK_NEXT
#define KEY_Page_Down   VK_NEXT
#define KEY_End         VK_END
#define KEY_Begin       0
#define KEY_Select      VK_SELECT
#define KEY_Print       VK_PRINT
#define KEY_Execute     VK_EXECUTE
#define KEY_Insert      VK_INSERT

#define KEY_F1          VK_F1
#define KEY_F2          VK_F2
#define KEY_F3          VK_F3
#define KEY_F4          VK_F4
#define KEY_F5          VK_F5
#define KEY_F6          VK_F6
#define KEY_F7          VK_F7
#define KEY_F8          VK_F8
#define KEY_F9          VK_F9
#define KEY_F10         VK_F10
#define KEY_F11         VK_F11
#define KEY_F12         VK_F12

#define KEY_Shift       VK_SHIFT
#define KEY_Control     VK_CONTROL
#define KEY_Menu        VK_MENU  

#define KEY_Shift_L     VK_LSHIFT
#define KEY_Shift_R     VK_RSHIFT
#define KEY_Control_L   VK_LCONTROL
#define KEY_Control_R   VK_RCONTROL
#define KEY_Caps_Lock   VK_CAPITAL
#define KEY_Shift_Lock  0
#define KEY_Meta_L      VK_LWIN
#define KEY_Meta_R      VK_RWIN
#define KEY_Alt_L       VK_LMENU
#define KEY_Alt_R       VK_RMENU

#define KEY_Space       VK_SPACE
#define KEY_0           ('0')
#define KEY_1           ('1')
#define KEY_2           ('2')
#define KEY_3           ('3')
#define KEY_4           ('4')
#define KEY_5           ('5')
#define KEY_6           ('6')
#define KEY_7           ('7')
#define KEY_8           ('8')
#define KEY_9           ('9')
#define KEY_A           ('a')
#define KEY_B           ('b')
#define KEY_C           ('c')
#define KEY_D           ('d')
#define KEY_E           ('e')
#define KEY_F           ('f')
#define KEY_G           ('g')
#define KEY_H           ('h')
#define KEY_I           ('i')
#define KEY_J           ('j')
#define KEY_K           ('k')
#define KEY_L           ('l')
#define KEY_M           ('m')
#define KEY_N           ('n')
#define KEY_O           ('o')
#define KEY_P           ('p')
#define KEY_Q           ('q')
#define KEY_R           ('r')
#define KEY_S           ('s')
#define KEY_T           ('t')
#define KEY_U           ('u')
#define KEY_V           ('v')
#define KEY_W           ('w')
#define KEY_X           ('x')
#define KEY_Y           ('y')
#define KEY_Z           ('z')

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

#define KEY_Space       GDK_KEY_space
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
#define KEY_A           GDK_KEY_a
#define KEY_B           GDK_KEY_b
#define KEY_C           GDK_KEY_c
#define KEY_D           GDK_KEY_d
#define KEY_E           GDK_KEY_e
#define KEY_F           GDK_KEY_f
#define KEY_G           GDK_KEY_g
#define KEY_H           GDK_KEY_h
#define KEY_I           GDK_KEY_i
#define KEY_J           GDK_KEY_j
#define KEY_K           GDK_KEY_k
#define KEY_L           GDK_KEY_l
#define KEY_M           GDK_KEY_m
#define KEY_N           GDK_KEY_n
#define KEY_O           GDK_KEY_o
#define KEY_P           GDK_KEY_p
#define KEY_Q           GDK_KEY_q
#define KEY_R           GDK_KEY_r
#define KEY_S           GDK_KEY_s
#define KEY_T           GDK_KEY_t
#define KEY_U           GDK_KEY_u
#define KEY_V           GDK_KEY_v
#define KEY_W           GDK_KEY_w
#define KEY_X           GDK_KEY_x
#define KEY_Y           GDK_KEY_y
#define KEY_Z           GDK_KEY_z

#endif /* _GTK */

#endif /* __KEY_CODES_H__ */