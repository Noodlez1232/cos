#include <input/term.h>
#include <input/keyboard.h>
#include <display/term.h>
#include <input/terminalcommands.h>

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wunused-parameter"

//Initalize our keyboard buffer to be used
//MAX_KEYBOARD_BUFFER defined in input/term.h
unsigned char keyboardbuffer[MAX_KEYBOARD_BUFFER];
//Used to store the current index we are at in the buffer
uint16_t keyboardbuffersize = 0;

//Used to store the current handler ID for our input handler
uint32_t keyboardhandlerID = 0;


//TERMINAL_MAX_COMMANDS defined in input/term.h
bool echokeys = true;

//Used to store all the strings for the commands
char *command_list[TERMINAL_MAX_COMMANDS];
//Used to store all the function pointers to use for the commands
command_t command_funcs[TERMINAL_MAX_COMMANDS];
//Used to be able to add commands as we please
bool command_funcs_allocation_table[TERMINAL_MAX_COMMANDS];

//Our string to be used as a prompt (temporary)
const char* prompt_str = "CS>"; 

//Initalize our input so that it can start adding to our buffer
void terminal_inputinitalize()
{
	terminal_bootInfo("Terminal starting\n", 2);
	//Initalize our keboard handler
	keyboardhandlerID = keyboard_installhandler(&terminal_inputhandler);
	//Initalize all our internal commands
	terminal_initcommandfuncs();
	terminal_writeline("Welcome to Crappy OS's Crappy Shell (CS)");
	terminal_writestring(prompt_str);
}


//Installs a command for the terminal
void terminal_installcommand(char *name, command_t func)
{
	//Iterates through all the commands currently installed
	for (uint32_t i = 0; i<=TERMINAL_MAX_COMMANDS - 1; i++)
	{
		//Check to see if there's an open spot in the command table
		if (!command_funcs_allocation_table[i])
		{
			//There is so we set the function pointer and the name pointers in the tables
			command_funcs[i]=func;
			command_list[i]=name;
			//We make sure we tell it that there is a command installed now
			command_funcs_allocation_table[i]=true;
			break;
		}
	}
}

//Uninstalls the input handler
void terminal_inputuninstall()
{
	keyboard_uninstallhandler(keyboardhandlerID);
}

//Our input handler for whenever a key is pressed
void terminal_inputhandler(unsigned char scancode)
{
	//Bounds checking is always important
	if (keyboardbuffersize - 1>=MAX_KEYBOARD_BUFFER)
	{
		return;
	}

	//If a backspace is pressed, we backspace visually and remove a character from the terminal buffer
	if (scancode==BACKSPACE)
	{
		keyboardbuffer[keyboardbuffersize]=0;
		//Bounds checking is very important
		if (keyboardbuffersize==0)
		{
			return;
		}
		keyboardbuffersize--;
		terminal_putchar('\b');
		terminal_updatecursor();
		return;
	}
	//If enter is pressed, we process the command and then clear the keyboard buffer and go to a new line
	if (scancode==ENTER)
	{
		BochsConsolePrintChar('c');
		if (echokeys)
			terminal_putchar('\n');
		terminal_parse_command(&keyboardbuffer);
		terminal_clearkeyboardbuffer();
		return;
	}
	//If no important character was pressed, ignore it.
	if (keyboard_scan_to_ascii(scancode)=='\0')
	{
		return;
	}
	//We store the key just pressed and increment our current index (Stored in keyboardbuffersize)
	keyboardbuffer[keyboardbuffersize]=keyboard_scan_to_ascii(scancode);
	keyboardbuffersize++;
	//If echokeys is a thing that is enabled, do such
	if (echokeys)
		terminal_putchar(keyboard_scan_to_ascii(scancode));
	terminal_updatecursor();
}

//Parse the command and check if it exists
void terminal_parse_command(char *command)
{

	//Create a temporary buffer to command so that we don't screw anything up
	char tmp[strlen(command)];
	memcpy(&tmp, command, strlen(command));
	//Iterate through the whole command until I reach a space or end of the string
	uint32_t j = 0;
	while (tmp[j]!=' ' && !(j>=strlen(command)))
	{
		j++;
	}
	//Terminate the string there
	tmp[j]='\0';


	//Go through all the current commands
	for (uint32_t i=0; i<TERMINAL_MAX_COMMANDS - 1; i++)
	{
		//Check if the command matches anything in the list
		if (strcmp(&tmp, command_list[i]))
		{
			//Check to make sure that command is still installed
			if (command_funcs_allocation_table[i])
			{
				//Run the command (passing *command)
				command_t handler;
				handler=command_funcs[i];
				handler(command);
				terminal_updatecursor();
				terminal_writestring(prompt_str);
				return;
			}
		}
	}
	//The command wasn't found, so we alert the user and get the hell out of there
	terminal_writeline("Command not found!");
	terminal_writestring(prompt_str);
	return;
}

void terminal_clearkeyboardbuffer()
{
	//Clear the keyboard buffer
	for (uint32_t i = 0; i<MAX_KEYBOARD_BUFFER -1; i++)
		keyboardbuffer[i]=0;
	//Set the buffer index to 0 also
	keyboardbuffersize=0;
}

//Gets the number of arguments in the command
uint32_t getnumargs(char *command)
{
	//Basically just loop through the string until we encounter a space character
	//Once we do, increment our numargs variable
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