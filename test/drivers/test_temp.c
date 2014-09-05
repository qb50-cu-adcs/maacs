/* ***********************************************************************
 * file: 		test_temp.c
 * author: 	jacob cook
 * created on: 5/27/2014
 * 
 * This is a template for a module/device driver testing menu.  To 
 * use, the directory name and "if else" statement needs to be added
 * to the main menu in test.c. 
 * **********************************************************************/	

#include <string.h>
#include "uart0.h"
#include "test.h"
#include "types.h"
#include "io.h"


/* current directory */
static char cdir[] = "maacs/temp$ ";

/* command list */
#define NCMDS (3)
const static char *cmd_list[NCMDS] = {"cmd1","cmd2","cmd3"};
static char dir_cmd[CMD_LEN];
static int exit_dir;

/* static function protorypes */
static void process_cmd(char *cmd);
static void cmd1_handler(char *arg);
static void cmd2_handler(char *arg);
static void cmd3_handler(char *arg);


/** temp_dir *************************************************************
 * top level function of the directory, shouldn't need to change anything
 * in here when making a new module. The new function handler name 
 * should be added to test.h.
 ************************************************************************/
void temp_dir(void)
{
	char *cmd = dir_cmd;
	exit_dir = 0; /* reset exit condition */
	while(!exit_dir)
	{
		uart0_puts(cdir);
		get_cmd(cmd, CMD_LEN);
		uart0_puts("\n\r");
		process_cmd(cmd);
	}
}

/** process_cmd **********************************************************
 * This is a static function ment to handle the spacific module testing
 * functions.  New commands should be added into the else if statement
 * it handle any additional functionality required for the test.
 ************************************************************************/
static void process_cmd(char *cmd)
{
	char *arg;
	
	/* separate command from argument */
	parse_command(cmd, &arg);

	if(!strcmp(cmd,(char *)cmd_list[0]))
	{
		/* command 1 */
		cmd1_handler(arg);
	}
	else if(!strcmp(cmd,(char *)cmd_list[1]))
	{
		/* command 2 */
		cmd2_handler(arg);
	}
	else if(!strcmp(cmd,(char *)cmd_list[2]))
	{
		/* command 3 */
		cmd3_handler(arg);
	}
	else if(!strcmp(cmd,"ls"))
	{
		list_dir((char **)cmd_list, NCMDS);
	}
	else if(!strcmp(cmd,"cd"))
	{
		/* for now we can only go backward */
		if(!strcmp(arg,".."))
		{
			exit_dir = 1;
		}
		else uart0_puts("Directory not Recognized\n\r");
	}
	else	
	{
		/* error message */
		uart0_puts("Command not recognized\n\r");
	}
}

/** cmd1_handler *********************************************************
 * the following function handles the command input from the user and 
 * executes the desired action. the variable arg is passed in as a raw 
 * string and may need further parsing with in the function to execute 
 * correctly.  It should not be assumes it is the desired form.
 ************************************************************************/
static void cmd1_handler(char *arg)
{
	uart0_puts("temp command 1 entered\n\r");
}

/** cmd2_handler *********************************************************
 * the following function handles the command input from the user and 
 * executes the desired action. the variable arg is passed in as a raw 
 * string and may need further parsing with in the function to execute 
 * correctly.  It should not be assumes it is the desired form.
 ************************************************************************/
static void cmd2_handler(char *arg)
{
	uart0_puts("temp command 2 entered\n\r");
}

/** cmd3_handler *********************************************************
 * the following function handles the command input from the user and 
 * executes the desired action. the variable arg is passed in as a raw 
 * string and may need further parsing with in the function to execute 
 * correctly.  It should not be assumes it is the desired form.
 ************************************************************************/
static void cmd3_handler(char *arg)
{
	uart0_puts("temp command 3 entered\n\r");
}

