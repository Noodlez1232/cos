#include <localization/en_US.h>
#include <input/keyboard.h>
#include <display/term.h>
#include <sys/irqs.h>
/*
 *In order to make sure the unused parameter regs_t *r doesn't raise
 *an unused parameter warning (it's kinda annoying), we tell the compiler
 *to just not do it. Problem solved...
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

bool shift=false;
bool caps=false;

keyboardHandler keyboard_handlers[256];

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{

	terminal_writeline("Keyboard handler called");

	unsigned char scancode;

	/* Read from the keyboard's data buffer */
	scancode = inportb(0x60);


	for (unsigned char i = 0; i>256; i++)
	{
		if (keyboard_handlers[i]!=0)
		{
			keyboardHandler handler = keyboard_handlers[i];
			handler(scancode);
		}
	}
}

void random_handler(unsigned char scancode)
{
	terminal_writeline("Random handler called");
	terminal_writeline(keyboard_scan_to_ascii(scancode));
}

char keyboard_scan_to_ascii(unsigned char scancode)
{
	if (scancode & 0x80)
	{
		if (scancode == LSHIFT + 0x80 || scancode == RSHIFT + 0x80)
		{
			shift=false;
			return '\0';
		}
	}
	else
	{
		if (scancode & LSHIFT || scancode & RSHIFT)
		{
			shift=true;
			return '\0';
		}
		if (scancode> numrowstart && scancode < numrowend)
		{
			//!A != !B is logical XOR
			if (!shift != !caps)
			{
				return shiftnumrow[scancode-numrowstart];
			}
			return numrow[scancode-numrowstart];
		}
		if (scancode > toprowstart && scancode < toprowend)
		{
			//!A != !B is logical XOR
			if (!shift != !caps)
			{
				return shifttoprow[scancode-toprowstart];
			}
			return toprow[scancode-toprowstart];
		}
		if (scancode > midrowstart && scancode < midrowend)
		{
			//!A != !B is logical XOR
			if (!shift != !caps)
			{
				return shiftmidrow[scancode-midrowstart];
			}
			return midrow[scancode-midrowstart];
		}
		if (scancode > bottomrowstart && scancode < bottomrowend)
		{
			//!A != !B is logical XOR
			if (!shift != !caps)
			{
				return shiftbottomrow[scancode-bottomrowstart];
			}
			return bottomrow[scancode-bottomrowstart];
		}
	}
	return '\0';
}


void keyboard_install()
{
	irq_install_handler(1, &keyboard_handler);
	install_keyhandler(0, &random_handler);
	terminal_bootInfo("Keyboard installed\n", 0);
}

void install_keyhandler(int number, keyboardHandler handler)
{
	keyboard_handlers[number]=handler;
}