#include <display/term.h>
#include <common.h>
#include <arch/x86/port.h>

//These variables store the sizes of the character display in character columns and rows
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

//This holds the current row we are on in the terminal
size_t terminal_row;
//This holds the current column we are on in ther terminal
size_t terminal_column;
//This stores the current color
uint8_t terminal_color;
//This stores the current foreground color
uint8_t terminal_foreground_color;
//This stores the current background color
uint8_t terminal_background_color;

//This is a pointer to where the current buffer for the terminal is
uint16_t* terminal_buffer;

//This stores all the characters required for the terminal_writehex* functions
char hextable[] = "0123456789ABCDEF";

//This stores the port's address for the video's control
uint16_t* terminal_video_port;


//Gets a VGA color

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	//the bg color is the last 4 bits of the color
	return fg | bg << 4;
}

//Gets a VGA character

uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	//A VGA character is 16 bits
	//The first 8 bits is the character code in ASCII
	//The second 8 bits is the color code which can be obtained through vga_entry_color()
	return (uint16_t) uc | (uint16_t) color << 8;
}

//This initalizes our terminal

void terminal_initialize(void)
{
	//This sets our current row to 0 (top of the screen)
	terminal_row = 0;
	//This sets our column to 0 (left side of the screen)
	terminal_column = 0;
	//This sets the default color of our terminal (Light grey for the foreground and black for the background)
	terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	//This sets the address of the terminal buffer
	terminal_buffer = (uint16_t*) 0xB8000;
	//terminal_buffer = (uint16_t*) 0xC00B8000;
	//terminal_buffer = (uint16_t*) 0xC03FF000;
	//This sets the address of the video port
	//terminal_video_port = (uint16_t*) 0x0463;
	//videoPort = (uint16_t*) 0xC0000463;

	//This clears the whole screen by replacing each entry with spaces
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			//To get the address of where we are in the buffer, we take Y and multiply that by the width of the screen (think about it as a table)
			//and we add our X coordinate to get the address we want to put our character in
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

//This sets the current color of the terminal

void terminal_set_color(enum vga_color fg, enum vga_color bg)
{
    terminal_foreground_color = fg;
    terminal_background_color = bg;
    terminal_color = vga_entry_color(fg, bg);
}

void terminal_set_background_color(enum vga_color bg)
{
    terminal_background_color = bg;
    terminal_color = vga_entry_color(terminal_foreground_color, bg);
}

void terminal_set_foreground_color(enum vga_color fg)
{
    terminal_foreground_color = fg;
    terminal_color = vga_entry_color(fg, terminal_background_color);
}



//This puts a vga entry at the coordinates specified and the VGA color given

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	//To get the address of where we are in the buffer, we take Y and multiply that by the width of the screen (think about it as a table)
	//and we add our X coordinate to get the address we want to put our character in
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

//This puts a character in the terminal in TTY mode (Basically does all the stuff you'd expect a terminal to do automatically)

void terminal_putchar(char c)
{
	//Just ignore a null character
	if (c == '\0')
		return;
	//Do a backspace if a backspace character is given
	if (c == '\b')
	{
		//In order to do a backspace, we basically do exactly what it sounds like
		//We go back one character, and we put a space right there.

		//Check to see if the column that we are currently at is 0 and if so, we just ignore it
		if (terminal_column == 0)
			return;
		//We subtract where we are in the terminal by one
		terminal_column--;
		//And now we put a space where we currently are to clear the character we are at
		terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
		return;
	}
	//Handle a newline character
	if (c == '\n')
	{
		//We set the column to 0 so that it goes back to the beginning of the buffer
		terminal_column = 0;
		//We increase the current row we are on
		terminal_row++;
		//Then we check to see if the row exceeds the maximum height
		if (terminal_row == VGA_HEIGHT)
			//If we did exceed the height, we scroll the terminal
			terminal_scroll();
	}//If the characters do not match any of those, we just put shove the character down and check for boundaries and crap like that
	else
	{
		//First we put the character at the current spot we are
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		//We increment the current column we are on
		terminal_column++;
		//Then we check to make sure that we stay in bounds of the screen
		if (terminal_column == VGA_WIDTH)
		{
			//If we did exceed it, we basically just do the same exact thing as the new line
			terminal_column = 0;
			terminal_row++;
			if (terminal_row == VGA_HEIGHT)
				terminal_scroll();
		}
	}

}

//Update the cursor to be at the current position of the terminal's character buffer

void terminal_updatecursor()
{
#if 0
	//We get the current position of the cursor and where we want it (done in the same fashion as we did in the terminal_putentryat() function)
	uint16_t position = (terminal_row * VGA_WIDTH) + terminal_column;
	//Set the cursor port to the LOW byte of the INDEX register
	outportb(terminal_video_port[0], 0x0F);
	//Now we actually send the low byte of the position to the INDEX register of the video port's cursor port
	outportb(terminal_video_port[0] + 1, (uint8_t) (position & 0xFF));
	//Set the cursor port to the HIGH byte of the INDEX register
	outportb(terminal_video_port[0], 0x0E);
	//And now we send the higher byte of the position to the INDEX register
	outportb(terminal_video_port[0] + 1, (uint8_t) ((position >> 8)&0xFF));
#else
	//We get the current position of the cursor and where we want it (done in the same fashion as we did in the terminal_putentryat() function)
	uint16_t position = (terminal_row * VGA_WIDTH) + terminal_column;
	//Set the cursor port to the LOW byte of the INDEX register
	outportb(0x3D4, 0x0F);
	//Now we actually send the low byte of the position to the INDEX register of the video port's cursor port
	outportb(0x3D4 + 1, (uint8_t) (position & 0xFF));
	//Set the cursor port to the HIGH byte of the INDEX register
	outportb(0x3D4, 0x0E);
	//And now we send the higher byte of the position to the INDEX register
	outportb(0x3D4 + 1, (uint8_t) ((position >> 8)&0xFF));
#endif
}

//Write a set of character data in TTY data according to the data sent and the size given

void terminal_write(char* data, size_t size)
{
	//We loop through every single character of the data buffer given until we reach that size given
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
	//At the end of writing such, we update the character position. This is much more efficient and less error prone than sending the update
	//every single character sent
	terminal_updatecursor();
}

//Write a string given to the terminal

void terminal_writestring(char* data)
{
	//We basically just call the terminal_write() function except we use strlen to get the length of the string given
	terminal_write(data, strlen(data));
}
//Write a string given to the terminal and write a newline character after

void terminal_writeline(char* data)
{
	terminal_writestring(data);
	terminal_putchar('\n');
}


//Scrolls the terminal

void terminal_scroll()
{
	//0 out the top line
	for (uint32_t i = 0; i < VGA_WIDTH; i++)
		//To 0 out the top line, we don't actually write null characters throughout the top line, but write spaces
		terminal_buffer[i] = vga_entry(' ', terminal_color);
	//Copy every single line after that to the line above
	//We get this value in the loop by calculating the length of the full terminal's buffer (-1 because copying over the top line can result
	//in overwriting values outside of the buffer)
	for (uint32_t i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++)
		terminal_buffer[i] = terminal_buffer[i + VGA_WIDTH];
	//Clear the bottom line (Just overwrite it using spaces)
	for (uint32_t i = 0; i < VGA_WIDTH; i++)
		//We get that value in the bottom by taking the height of the terminal and subtract it by one (which in turn equals the bottom line)
		//then using a modified version of the formula used in vga_putentryat()
		terminal_buffer[i + ((VGA_HEIGHT - 1) * VGA_WIDTH)] = vga_entry(' ', terminal_color);
	//Now we set the row to the bottom line
	terminal_row = VGA_HEIGHT - 1;
	//And we set the column to the left side of the screen
	terminal_column = 0;
}

//Clears the terminal screen

void terminal_clear()
{
	//We clear the terminal by writing spaces throughout the full buffer
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			//To get the address of where we are in the buffer, we take Y and
			//multiply that by the width of the screen (think about it as a
			//table) and we add our X coordinate to get the address we want to
			//put our character in
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	//Then we reset the cursor's position to 0,0 (top left)
	terminal_setcursor(0, 0);
}

//Sets the cursor's current position

void terminal_setcursor(size_t x, size_t y)
{
	//Set the current column to the x coordinate given
	terminal_column = x;
	//Set the current row to the y coordinate given
	terminal_row = y;
	//And of course we update where the display's cursor is currently at
	terminal_updatecursor();
}

//Here we send a newline to the console

void terminal_newline()
{
	//Instead of calling another function to handle a newline, I just copied it
	//here, for increased speeds

	//This same code can be found in terminal_putchar()
	terminal_column = 0;
	if (++terminal_row == VGA_HEIGHT)
		terminal_scroll();
}

//Show a boot information log thingy

void terminal_info(char* string, char infoLevel)
{

	switch (infoLevel)
	{
			//Okay
		case 0:
			terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
			terminal_writestring("OK");
			break;
			//Error
		case 1:
			terminal_color = vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
			terminal_writestring("ERR");
			break;
			//Info
		case 2:
			terminal_color = vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK);
			terminal_writestring("INFO");
			break;
			//Unknown
		case 3:
			terminal_color = vga_entry_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
			terminal_writestring("WARN");
			break;
		default:
			terminal_color = vga_entry_color(VGA_COLOR_BROWN, VGA_COLOR_BLACK);
			terminal_writestring("?");
			break;
	}
	terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_writestring("] ");
	terminal_writestring(string);

}

void terminal_warn(char* string)
{
    terminal_writestring("[");
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    terminal_writestring("WARN");
}

//Write a byte's value in hexadecimal

void terminal_writehexbyte(uint8_t value)
{
	//Write higher nibble by refrencing the hex table
	//(One hex character =one nibble)
	terminal_putchar(hextable[value >> 4]);
	//Write lower nibble
	terminal_putchar(hextable[value & 0xF]);
}

//Write a word's value in hexadecimal

void terminal_writehexword(uint16_t value)
{
	//Write higher byte
	terminal_writehexbyte(value >> 8);
	//Write lower byte
	terminal_writehexbyte(value & 0xFF);
}

//Write a dword's (dword=32bits or an int) value

void terminal_writehexdword(uint32_t value)
{
	//Write higher word
	terminal_writehexword(value >> 16);
	//Write lower word
	terminal_writehexword(value & 0xFFFF);
}

void terminal_itoa(uint32_t value)
{
	char buffer [12];
	int i = 0;

	unsigned int n1 = value;

	while (n1 != 0)
	{
		buffer [i++] = n1 % 10 + '0';
		n1 = n1 / 10;
	}

	buffer [i] = '\0';

	for (int t = 0; t < i / 2; t++)
	{
		buffer [t] ^= buffer [i - t - 1];
		buffer [i - t - 1] ^= buffer [t];
		buffer [t] ^= buffer [i - t - 1];
	}

	if (value == 0)
	{
		buffer [0] = '0';
		buffer [1] = '\0';
	}

	terminal_writestring(buffer);
}


//Our debug functions
#if DEBUG == 1

void terminal_debug_putchar(char c)
{
	//No Virtual machine
#    if VM == 0
	//Not implemented yet
}
//Bochs
#    elif VM == 1
	//Use the bochs functions
	outportb(0xe9, c);
}
//QEMU
#    elif VM == 2
	//stub
}
//Other
#    elif VM == 3
	//We use the serial's putchar function
	serial_putchar(c);
}
#    else
} //End of terminal_debug_putchar()
#    endif

void terminal_debug_write(char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_debug_putchar(data[i]);
}

void terminal_debug_writestring(char* data)
{
	terminal_debug_write(data, strlen(data));
}

void terminal_debug_writeline(char* data)
{
	terminal_debug_writestring(data);
	terminal_debug_putchar('\n');
}

void terminal_debug_writehexbyte(uint8_t value)
{
	//Write higher nibble
	terminal_debug_putchar(hextable[value >> 4]);
	//Write lower nibble
	terminal_debug_putchar(hextable[value & 0xF]);
}

void terminal_debug_writehexword(uint16_t value)
{
	//Write higher byte
	terminal_debug_writehexbyte(value >> 8);
	//Write lower byte
	terminal_debug_writehexbyte(value & 0xFF);
}

void terminal_debug_writehexdword(uint32_t value)
{
	//Write higher word
	terminal_debug_writehexword(value >> 16);
	//Write lower word
	terminal_debug_writehexword(value & 0xFFFF);
}

void terminal_debug_itoa(uint32_t value)
{
	char buffer [12];
	int i = 0;

	unsigned int n1 = value;

	while (n1 != 0)
	{
		buffer [i++] = n1 % 10 + '0';
		n1 = n1 / 10;
	}

	buffer [i] = '\0';

	for (int t = 0; t < i / 2; t++)
	{
		buffer [t] ^= buffer [i - t - 1];
		buffer [i - t - 1] ^= buffer [t];
		buffer [t] ^= buffer [i - t - 1];
	}

	if (value == 0)
	{
		buffer [0] = '0';
		buffer [1] = '\0';
	}

	terminal_debug_writestring(buffer);
}
#endif
