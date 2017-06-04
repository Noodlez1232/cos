#ifndef TERMINPUT_H
#define TERMINPUT_H 1

#include <common.h>

#define MAX_KEYBOARD_BUFFER 1024

unsigned char* terminal_getline();
void terminal_inputhandler(unsigned char scancode);
void terminal_inputinitalize();
void terminal_inputuninstall();
void terminal_parse_command();

#endif