#include <display/term.h>
#include <common.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

#define MAX_KEYBOARD_BUFFER 1024

char hextable[] = "0123456789ABCDEF";

uint16_t* videoPort;


//Gets a VGA color
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}
 
 //Gets a VGA character
uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	videoPort=(uint16_t*) 0x0463;

	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(enum vga_color fg, enum vga_color bg)
{
	terminal_color=vga_entry_color(fg, bg);
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c)
{
	if (c == '\0')
		return;
	if (c == '\b')
	{
		if (terminal_column==0)
			return;
		terminal_column--;
		terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
		return;
	}
	if (c == '\n')
	{
		terminal_column=0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_scroll();
	}
	else
	{
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH)
		{
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT)
				terminal_scroll();
		}
	}
}
void terminal_updatecursor()
{
	uint16_t position = (terminal_row*VGA_WIDTH) + terminal_column;
	//Set the cursor to the LOW value in the INDEX register
	outportb(videoPort[0], 0x0F);
	outportb(videoPort[0]+1, (uint8_t)(position&0xFF));
	//Set the cursor to the HIGH value in the INDEX register
	outportb(videoPort[0], 0x0E);
	outportb(videoPort[0]+1, (uint8_t)((position>>8)&0xFF));
}


void terminal_write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
	terminal_updatecursor();
}
 
void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}
void terminal_writeline(const char* data)
{
	terminal_writestring(data);
	terminal_putchar('\n');
}


//Scrolls the terminal
void terminal_scroll()
{
	//0 out the top line
	for (uint32_t i = 0; i<VGA_WIDTH; i++)
		terminal_buffer[i] = vga_entry(' ', terminal_color);
	//Copy every single line after that to the line above
	for (uint32_t i = 0; i<(VGA_HEIGHT-1)*VGA_WIDTH; i++)
		terminal_buffer[i]=terminal_buffer[i+VGA_WIDTH];
	//Clear the bottom line
	for (uint32_t i = 0; i<VGA_WIDTH; i++)
		terminal_buffer[i+((VGA_HEIGHT-1)*VGA_WIDTH)] = vga_entry(' ', terminal_color);
	terminal_row=VGA_HEIGHT-1;
	terminal_column=0;
	
	//TODO: Fix this. It's absolute shit...
}

void terminal_clear()
{
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	terminal_setcursor(0,0);
}

void terminal_setcursor(size_t x, size_t y)
{
	terminal_column = x;
	terminal_row = y;
	terminal_updatecursor();
}

void terminal_newline()
{
	terminal_column=0;
	if (++terminal_row == VGA_HEIGHT)
		terminal_scroll();
}

void terminal_bootInfo(const char* string, char infoLevel)
{
	terminal_writestring("[");
	switch (infoLevel)
	{
		case 0:
			terminal_color=vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
			terminal_writestring("OK");
			break;
		case 1:
			terminal_color=vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
			terminal_writestring("ERR");
			break;
		case 2:
			terminal_color=vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK);
			terminal_writestring("INFO");
			break;
		default:
			terminal_color=vga_entry_color(VGA_COLOR_BROWN, VGA_COLOR_BLACK);
			terminal_writestring("?");
			break;
	}
	terminal_color=vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_writestring("] ");
	terminal_writestring(string);
	
}

void terminal_writehexbyte(uint8_t value)
{
	//Write higher nibble
	terminal_putchar(hextable[value >> 4]);
	//Write lower nibble
	terminal_putchar(hextable[value & 0xF]);
}

void terminal_writehexword(uint16_t value)
{
	//Write higher byte
	terminal_writehexbyte(value >> 8);
	//Write lower byte
	terminal_writehexbyte(value & 0xFF);
}

void terminal_writehexdword(uint32_t value)
{
	//Write higher word
	terminal_writehexword(value >> 16);
	//Write lower word
	terminal_writehexword(value & 0xFFFF);
}
