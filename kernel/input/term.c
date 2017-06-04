#include <input/term.h>
#include <input/keyboard.h>
#include <display/term.h>

//Initalize our keyboard buffer to be used
unsigned char keyboardbuffer[MAX_KEYBOARD_BUFFER];
uint16_t keyboardbuffersize = 0;
uint32_t keyboardhandlerID = 0;

bool echo_keys = true;

char *command_list[] = 
{
	"hello",
	"goodbye"
};

unsigned char* terminal_getline()
{
	while(keyboard_scan_to_ascii(keyboardbuffer[keyboardbuffersize-1])!='\n')
		;
	//Done giving input
	//0 out the current buffer
	for (int i = 0; i<MAX_KEYBOARD_BUFFER; i++)
	{
		keyboardbuffer[i]=0;
	}
	return 0;
}

//Initalize our input so that it can start adding to our buffer
void terminal_inputinitalize()
{
	keyboardhandlerID = keyboard_installhandler(&terminal_inputhandler);
}

void terminal_inputuninstall()
{
	keyboard_uninstallhandler(keyboardhandlerID);
}

void terminal_inputhandler(unsigned char scancode)
{
	BochsConsolePrintChar('i');
	//Bounds checking is always important
	if (keyboardbuffersize - 1>=MAX_KEYBOARD_BUFFER)
	{
		return;
	}

	if (scancode==ENTER)
	{
		BochsConsolePrintChar('c');
		keyboardbuffer[++keyboardbuffersize]='\0';
		terminal_parse_command();
	}
	keyboardbuffer[++keyboardbuffersize]=keyboard_scan_to_ascii;
	terminal_putchar(keyboard_scan_to_ascii(scancode));
}

void terminal_parse_command()
{
	BochsBreak();
	if (memcmp(command_list[0], &keyboardbuffer, strlen(command_list[0]))==0)
	{
		BochsConsolePrintChar('d');
	}
	else
	{
		terminal_writeline("Command not found!");
	}
	return;
}