#ifndef _INTCOMMANDS_H
#define _INTCOMMANDS_H 1

#include <common.h>


void terminal_initcommandfuncs();

void terminal_internalcommand_echo(char *command);
void terminal_internalcommand_hello(char *command);
void terminal_internalcommand_cls(char *command);

#endif