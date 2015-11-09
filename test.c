#include<stdlib.h> 
#include<stdio.h>

#include"libanykey.h"

int error(char*msg) { perror(msg); return 1; }
int main(int argc, char*args[])
{
	struct anykey*anykey;
	anykey = anykey_create();
	if(anykey==NULL)
		return error("anykey_create()");

	char*key;
	for(;;)
	{
		key = anykey_getkey(anykey);
		printf("key: %s\n",key);

		if(key!=NULL && !strcmp(key,"KeyUp Escape"))
			break;
	}

	anykey_delete(anykey); 
	return 0;
}

