#include<stdlib.h> 
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<signal.h>
#include<unistd.h>

#include"libanykey.h"

int error(char*msg) { perror(msg); return 1; }

int help(int err)
{
	printf("                                readanykey\n"
	       "                                **********\n"
	       "readanykey is a command-line utility for reading KeyUp/KeyDown events\n"
	       "in terminal software.  all keypress events are reported as newline delimited\n"
	       "strings.  it allows for much more flexible keyboard input then terminfo or\n"
	       "curses based software, but only works in software terminals running under\n"
	       "X Windows (like xterm, konsole, etc).\n"
	       "\n"
	       "to test readanykey, type:\n"
               " $ readanykey\n"
	       "this will print all KeyUp/KeyDown events to the screen.  pressing Escape will\n"
	       "quit readanykey.\n"
	       "\n"
	       "to use a different key event to quit readanykey, type:\n"
	       " $ readanykey -e 'KeyUp <different-key>'\n"
	       "\n"
	       "to run readanykey int the terminal without a quit-key, type:\n"
	       " $ readanykey -e ''\n"
	       "(but be warned, ctrl-z, ctrl-c, and ctrl-\\ will not work, so you will have to\n"
	       " close your software terminal to exit).\n"
	       "\n"
	       "readanykey can be used in shell scripts, or via a popen() style function to\n"
	       "give any program access to KeyUp/KeyDown events.  when run in this environment\n"
	       "readanykey will default to the \"readanykey -e ''\" behaviour, and will only\n"
	       "exit once its stdout pipe has been closed.\n");
	return err;
}

int main(int argc, char*args[])
{
	char*exitkey;

	if(isatty(1))
		exitkey = "KeyUp Escape";
	else
		exitkey = NULL;
	/* FIXME should we prevent redirecting to a file without an exitkey? */

	for(++args;*args;++args)
		if(!strcmp(*args,"-h")||!strcmp(*args,"--help"))
			return help(0);
		else if(!strcmp(*args,"-e")||!strcmp(*args,"--exit-key"))
			exitkey=*(++args);

	struct anykey*anykey;
	anykey = anykey_create();
	if(anykey==NULL)
		return error("anykey_create()");

	signal(SIGPIPE,SIG_IGN);

	char*key;
	for(;;)
	{
		key = anykey_getkey(anykey);
		if(key==NULL)
			continue;

		printf("%s\n",key);
		if(errno==EPIPE)
			break;

		fflush(stdout);
		if(errno==EPIPE)
			break;

		if(exitkey!=NULL && !strcmp(key,exitkey))
			break;
	}

	anykey_delete(anykey);
	return 0;
}

