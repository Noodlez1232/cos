#ifndef _KEYB_H
#define _KEYB_H 1

#include <common.h>
typedef void (*keyboardHandler) (unsigned char scancode);
void keyboard_handler(struct regs *r);
void keyboard_install();
void random_handler(unsigned char scancode);
char keyboard_scan_to_ascii(unsigned char scancode);
void install_keyhandler(int number, keyboardHandler handler);
#endif