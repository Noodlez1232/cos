#include <input/term.h>
#include <input/keyboard.h>
#include <display/term.h>

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wunused-parameter"

//Initalize our keyboard buffer to be used
//MAX_KEYBOARD_BUFFER defined in input/term.h
unsigned char keyboardbuffer[MAX_KEYBOARD_BUFFER];
uint16_t keyboardbuffersize = 0;
uint32_t keyboardhandlerID = 0;


//TERMINAL_MAX_COMMANDS defined in input/term.h
bool echokeys = true;

char *command_list[TERMINAL_MAX_COMMANDS];
command_t command_funcs[TERMINAL_MAX_COMMANDS];
bool command_funcs_allocation_table[TERMINAL_MAX_COMMANDS];

//Initalize our input so that it can start adding to our buffer
void terminal_inputinitalize()
{
	keyboardhandlerID = keyboard_installhandler(&terminal_inputhandler);
	terminal_initcommandfuncs();
}

void terminal_initcommandfuncs()
{
	terminal_installcommand("hello", &terminal_internalcommand_hello);
	terminal_installcommand("echo", &terminal_internalcommand_echo);
	terminal_installcommand("cls", &terminal_internalcommand_cls);
}



/*
 *Internal Commands
 */
//Hello World
void terminal_internalcommand_hello(char *command)
{
	terminal_writeline("Hello World!");
}
//Echo
void terminal_internalcommand_echo(char *command)
{
	char *tmp = command;
	while (tmp[0]!=' ')
	{
		tmp++;
	}
	tmp++;
	terminal_writeline(tmp);
}
//Clear screen
void terminal_internalcommand_cls(char *command)
{
	terminal_clear();
}

void terminal_installcommand(char *name, command_t func)
{
	for (uint32_t i = 0; i<=TERMINAL_MAX_COMMANDS - 1; i++)
	{
		if (!command_funcs_allocation_table[i])
		{
			command_funcs[i]=func;
			command_list[i]=name;
			command_funcs_allocation_table[i]=true;
			break;
		}
	}
}

void terminal_inputuninstall()
{
	keyboard_uninstallhandler(keyboardhandlerID);
}

void terminal_inputhandler(unsigned char scancode)
{
	//Bounds checking is always important
	if (keyboardbuffersize - 1>=MAX_KEYBOARD_BUFFER)
	{
		return;
	}
	if (scancode==BACKSPACE)
	{
		keyboardbuffer[keyboardbuffersize]=0;
		keyboardbuffersize--;
		terminal_putchar('\b');
		terminal_updatecursor();
		return;
	}
	if (scancode==ENTER)
	{
		BochsConsolePrintChar('c');
		if (echokeys)
			terminal_putchar('\n');
		terminal_parse_command(&keyboardbuffer);
		terminal_clearkeyboardbuffer();
		return;
	}
	if (keyboard_scan_to_ascii(scancode)=='\0')
	{
		return;
	}
	keyboardbuffer[keyboardbuffersize]=keyboard_scan_to_ascii(scancode);
	keyboardbuffersize++;
	if (echokeys)
		terminal_putchar(keyboard_scan_to_ascii(scancode));
	terminal_updatecursor();
}

void terminal_parse_command(char *command)
{
	for (uint32_t i=0; i<TERMINAL_MAX_COMMANDS - 1; i++)
	{
		char tmp[strlen(command)];
		memcpy(&tmp, command, strlen(command));
		uint32_t j = 0;
		while (tmp[j]!=' ' && !(j>=strlen(command)))
		{
			j++;
		}
		tmp[j]='\0';
		if (strcmp(&tmp, command_list[i]))
		{
			if (command_funcs_allocation_table[i])
			{
				command_t handler;
				handler=command_funcs[i];
				handler(command);
				terminal_updatecursor();
				return;
			}
		}
	}
	terminal_writeline("Command not found!");
	return;
}

void terminal_clearkeyboardbuffer()
{
	//Clear the keyboard buffer
	for (uint32_t i = 0; i<MAX_KEYBOARD_BUFFER -1; i++)
		keyboardbuffer[i]=0;
	keyboardbuffersize=0;
}

uint32_t getnumargs(char *command)
{
	uint32_t numargs = 0;
	for (uint32_t i = 0; i<strlen(command); i++)
	{
		if (command[i]==' ')
		{
			numargs++;
		}
	}
	return numargs;

}