#include <input/terminalcommands.h>
#include <input/term.h>
#include <display/term.h>
#include <sys/timer.h>

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wunused-parameter"

void terminal_initcommandfuncs()
{
	//Install all the internal commands that are included in this file and that are basically required to run
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
	//This iterates through every single character in command until we get to a space, then we print everything after that
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

/*
void terminal_internalcommand_timertest(char *command)
{
	//We wait 100 seconds
	timer_delay(100);

}
*/

