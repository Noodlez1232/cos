#include <input/terminalcommands.h>
#include <input/term.h>
#include <display/term.h>
#include <sys/timer.h>
#include <sys/irqs.h>

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wunused-parameter"

void terminal_initcommandfuncs()
{
	//Install all the internal commands that are included in this file and that are basically required to run
	terminal_installcommand("hello", &terminal_internalcommand_hello);
	terminal_installcommand("echo", &terminal_internalcommand_echo);
	terminal_installcommand("cls", &terminal_internalcommand_cls);
	terminal_installcommand("easteregg", &terminal_internalcommand_easteregg);
	terminal_installcommand("test", &terminal_internalcommand_test);
	terminal_installcommand("timertest", &terminal_internalcommand_timertest);
}



/*
 *Internal Commands
 */

//Hello World

void terminal_internalcommand_hello(char *command)
{
	//This iterates through every single character in command until we get to a space, then we print everything after that
	char *tmp = command;
	while (tmp[0] != ' ')
	{
		tmp++;
	}
	tmp++;
	if (strcmp(tmp, "this is a test"))
	{
		terminal_writeline("Nice. You've tested it now. Go away, please.");
	} else
	{
		terminal_writeline("Hello World!");	
	}
}

//Echo

void terminal_internalcommand_echo(char *command)
{
	//This iterates through every single character in command until we get to a space, then we print everything after that
	char *tmp = command;
	while (tmp[0] != ' ')
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

void terminal_internalcommand_easteregg(char *command)
{
	terminal_writeline("Command not found! (Quit snooping around, please... It's not healthy)");
}

//Quick testing command

void terminal_internalcommand_test(char *command)
{
	//terminal_writehexdword(sizeof(size_t));
}

void terminal_internalcommand_timertest(char *command)
{
	//We wait 100 seconds
	timer_install(1000);
	timer_reset();
	timer_delay(100);

}

