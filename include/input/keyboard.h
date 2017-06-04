#ifndef _KEYB_H
#define _KEYB_H 1

#define KEYBOARD_MAX_HANDLERS 1024

#include <common.h>

#define LSHIFT 0x2A
#define RSHIFT 0x36
#define CAPSLOCK 0x3A
#define SPACE 0x39
#define ENTER 0x1C
#define BACKSPACE 0x0E


typedef void (*keyboardHandler) (unsigned char scancode);
void keyboard_handler(struct regs *r);
void keyboard_install();
void random_handler(unsigned char scancode);
char keyboard_scan_to_ascii(unsigned char scancode);

//Handler handlers (heh)
void keyboard_modifyhandler(uint32_t ID, keyboardHandler handler);
void keyboard_uninstallhandler(uint32_t ID);
uint32_t keyboard_installhandler(keyboardHandler handler);

#endif