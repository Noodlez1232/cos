//Localization is defined in keyboard.h
#include <input/keyboard.h>
#include <display/term.h>
#include <sys/irqs.h>
#include <input/localization/en_US.h>
/*
 *In order to make sure the unused parameter regs_t *r doesn't raise
 *an unused parameter warning (it's kinda annoying), we tell the compiler
 *to just not do it. Problem solved...
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

bool shift = false;
bool caps = false;

//KEYBOARD_MAX_HANDLERS is defined in keyboard.h

keyboardHandler keyboard_handlers[KEYBOARD_MAX_HANDLERS];
bool keyboard_handlers_allocation_table[KEYBOARD_MAX_HANDLERS];

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{

	unsigned char scancode;

	/* Read from the keyboard's data buffer */
	scancode = inportb(0x60);
	//Send the end of interrupt signal
#if DEBUG == 1
	terminal_debug_writeline("Sending EOI signal for keyboard");
#endif
	send_EOI(1);
	for (unsigned int i = 0; i <= KEYBOARD_MAX_HANDLERS - 1; i++)
	{
		if (keyboard_handlers_allocation_table[i])
		{
			keyboardHandler handler = keyboard_handlers[i];
			handler(scancode);
		}
	}
}

void random_handler(unsigned char scancode)
{
	char ret = keyboard_scan_to_ascii(scancode);
	terminal_putchar(ret);
	terminal_updatecursor();
}

char keyboard_scan_to_ascii(unsigned char scancode)
{
	//0x80 is the released bit
	if (scancode & 0x80)
	{
		//We add 0x80 so that we can make sure it is actually equal
		if (scancode == LSHIFT + 0x80 || scancode == RSHIFT + 0x80)
		{
			shift = !shift;
			return '\0';
		}
	}//The key was just pressed
	else
	{
		//All our special keys
		if (scancode == ENTER)
		{
			return '\n';
		}
		if (scancode == SPACE)
		{
			return ' ';
		}
		if (scancode == BACKSPACE)
		{
			return '\b';
		}
		if (scancode == CAPSLOCK)
		{
			caps = !caps;
		}
		if ((scancode == LSHIFT) || (scancode == RSHIFT))
		{
			shift = !shift;
			return '\0';
		}
		//Check if a number row key has been pressed
		if ((scancode >= numrowstart) && (scancode <= numrowend))
		{
			//!A != !B is logical XOR
			if (!shift != !caps)
			{
				return shiftnumrow[scancode - numrowstart];
			}
			return numrow[scancode - numrowstart];
		}
		//Check if the top row keys have been pressed at all
		if ((scancode >= toprowstart) && (scancode <= toprowend))
		{
			//!A != !B is logical XOR
			if (!shift != !caps)
			{
				return shifttoprow[scancode - toprowstart];
			}
			return toprow[scancode - toprowstart];
		}
		//Check if the middle row keys have been pressed at all
		if ((scancode >= midrowstart) && (scancode <= midrowend))
		{
			//!A != !B is logical XOR
			if (!shift != !caps)
			{
				return shiftmidrow[scancode - midrowstart];
			}
			return midrow[scancode - midrowstart];
		}
		//Check if the middle row keys have been pressed at all
		if ((scancode >= bottomrowstart) && (scancode <= bottomrowend))
		{
			//!A != !B is logical XOR
			if (!shift != !caps)
			{
				return shiftbottomrow[scancode - bottomrowstart];
			}
			return bottomrow[scancode - bottomrowstart];
		}
	}

	//Return a null character if none of these things apply
	return '\0';
}

//Install our keyboard

void keyboard_install()
{
	irq_install_handler(1, &keyboard_handler);
}

/* Keyboard handler handlers (kek) */

//Uninstalls the handler with the ID of ID

void keyboard_uninstallhandler(uint32_t ID)
{
	//Bounds checking is always important
	if (ID > KEYBOARD_MAX_HANDLERS - 1)
		return;
	keyboard_handlers[ID] = 0;
	keyboard_handlers_allocation_table[ID] = false;
}

//Sets the handler of handler ID ID to the handler handler

void keyboard_modifyhandler(uint32_t ID, keyboardHandler handler)
{
	//Once again, bounds checking is always important
	if (ID > KEYBOARD_MAX_HANDLERS - 1)
		return;
	keyboard_handlers[ID] = handler;
	keyboard_handlers_allocation_table[ID] = true;
}

/*
 *Installs a handler to the handler table
 *Returns your keyhandler ID. This is used when modifying or uninstalling
 *a keyhandler
 */
uint32_t keyboard_installhandler(keyboardHandler handler)
{
	for (uint32_t i = 0; i <= KEYBOARD_MAX_HANDLERS - 1; i++)
	{
		if (!keyboard_handlers_allocation_table[i])
		{
			keyboard_handlers[i] = handler;
			keyboard_handlers_allocation_table[i] = true;
			return i;
		}
	}
	return 0;
}
