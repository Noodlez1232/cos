#define COM1_PORT 0x3f8   /* COM1 */
#define DATA 0
#define INTERRUPT_ENABLE 1
#define INTERRUPT_ID 2
#define LINE_CONTROL 3
#define MODEM_CONTROL 4
#define LINE_STATUS 5

#define INTERRUPT_ENABLE_DATA_RECIEVED 0x01
#define INTERRUPT_ENABLE_TRANSMITTER_EMPTY 0x02
#define INTERRUPT_ENABLE_BREAK_ERROR 0x04
#define INTERRUPT_ENABLE_STATUS_CHANGE 0x08

#define LINE_CONTROL_DLAB 0x80
#define LINE_CONTROL_CHARACTER_BITS_5 0x00
#define LINE_CONTROL_CHARACTER_BITS_6 0x01
#define LINE_CONTROL_CHARACTER_BITS_7 0x02
#define LINE_CONTROL_CHARACTER_BITS_8 0x03
#define LINE_CONTROL_STOP_BITS_1 0x00
#define LINE_CONTROL_STOP_BITS_2 0x04	//This may also be 1.5 depending on your character bits
#define LINE_CONTROL_PARITY_BIT_NONE 0x00
#define LINE_CONTROL_PARITY_BIT_ODD 0x08
#define LINE_CONTROL_PARITY_BIT_EVEN 0x18
#define LINE_CONTROL_PARITY_BIT_MARK 0x28
#define LINE_CONTROL_PARITY_BIT_SPACE 0x38


#include <sys/serial.h>
#include <sys/irqs.h>
#include <display/term.h>



void serial_init()
{
   outportb(COM1_PORT + INTERRUPT_ENABLE, INTERRUPT_ENABLE_DATA_RECIEVED);    					// Disable all interrupts (Basically turning it into a read-only port)
   outportb(COM1_PORT + LINE_CONTROL, LINE_CONTROL_DLAB);    			// Enable DLAB (This is used for the baud rate divisor in the DATA port)
   outportb(COM1_PORT + DATA, 0x03);    								// Set divisor to 3 (lo byte) 38400 baud
   outportb(COM1_PORT + DATA + 1, 0x00);    							//                  (hi byte)
   outportb(COM1_PORT + LINE_CONTROL, LINE_CONTROL_CHARACTER_BITS_8);   // 8 bits, no parity, one stop bit
   outportb(COM1_PORT + INTERRUPT_ID, 0xC7);    						// Enable FIFO, clear them, with 14-byte threshold
   outportb(COM1_PORT + MODEM_CONTROL, 0x0B);    						// IRQs enabled, RTS/DSR set

   //set up the IRQ of the serial port
   irq_install_handler(4, &serial_handler);

   terminal_debug_writeline("Serial initialized");
}

int is_transmit_empty()
{
   return inportb(COM1_PORT + LINE_STATUS) & 0x20;
}
 
void serial_putchar(char c)
{
   while (is_transmit_empty() == 0);
 
   outportb(COM1_PORT + DATA, c);
}

void serial_handler(regs_t *r)
{
	char charRecieved = inportb(COM1_PORT + DATA);
}