#ifndef TERMINPUT_H
#define TERMINPUT_H 1

#include <common.h>

#define MAX_KEYBOARD_BUFFER 1024
#define TERMINAL_MAX_COMMANDS 1024

typedef void (*command_t) (char *command);

unsigned char* terminal_getline();
void terminal_inputhandler(unsigned char scancode);
void terminal_inputinitalize();
void terminal_inputuninstall();
void terminal_parse_command(char *command);
void terminal_installcommand(char *name, command_t func);
void terminal_clearkeyboardbuffer();
uint32_t getnumargs(char *command);



#endif