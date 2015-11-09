/* X11 */
#include<X11/Xlib.h>

/* POSIX */
#include<termios.h>
#include<poll.h>

/* C89 */
#include<stdlib.h> 
#include<stdio.h>

#include"libanykey.h"

static int anykey_charmode(int tty)
{
        struct termios data;

        tcgetattr(tty, &data);
        data.c_lflag &= ~ICANON;
        data.c_lflag &= ~ECHO;
	data.c_lflag &= ~ISIG;  /* disable C-c C-\ C-z */
	data.c_iflag &= ~IXON;  /* disable C-q C-s */
	data.c_iflag &= ~ICRNL;	/* disable translate \r to \n */
        data.c_cc[VMIN] = 0;
        data.c_cc[VTIME] = 0;
        tcsetattr(tty, TCSANOW, &data);

        return 0;
}
static int anykey_linemode(int tty)
{
	tcflush(tty,TCIFLUSH);

        struct termios data;

        tcgetattr(tty, &data);
        data.c_lflag |= ICANON;
        data.c_lflag |= ECHO;
	data.c_lflag |= ISIG;   /* enable C-c C-\ C-z */
	data.c_iflag |= IXON;   /* enable C-q C-s */ 
	data.c_iflag |= ICRNL;	/* enable translate \r to \n */
        data.c_cc[VMIN] = 1;
        data.c_cc[VTIME] = 0;
        tcsetattr(tty, TCSANOW, &data);

        return 0;
} 

static void anykey_listen(Display*display,Window window)
{
	Window root;
	Window parent;
	Window*windows;
	unsigned int count;

	XSelectInput(display,window,KeyPressMask|KeyReleaseMask);

	XQueryTree(display,window,&root,&parent,&windows,&count); 
	unsigned int i;
	for(i=0;i<count;++i)
		XSelectInput(display,windows[i],KeyPressMask|KeyReleaseMask);

	XFree(windows);
}

struct anykey
{
	Display*display;
	Window window;
	int xfd;
	int tty;
	char cache[128];
}; 
struct anykey*anykey_delete(struct anykey*anykey)
{
	if(anykey==NULL)
		return NULL;

	if(anykey->display!=NULL)
	{
		XCloseDisplay(anykey->display);
		anykey->display = NULL;
	}
	if(anykey->tty!=-1)
	{
		/* restore line buffering and keypress echo in tty */
		anykey_linemode(anykey->tty);
		anykey->tty = -1;
	}

	free(anykey);
	return NULL;
}
struct anykey*anykey_create(void)
{
	struct anykey*anykey;
	anykey = malloc(sizeof(struct anykey));
	if(anykey==NULL)
		return NULL; 
	anykey->display = NULL;
	anykey->window = None;
	anykey->xfd = -1;
	anykey->tty = -1;

	anykey->display = XOpenDisplay(NULL);
	if(anykey->display==NULL)
		return anykey_delete(anykey);
	anykey->xfd = ConnectionNumber(anykey->display);

	char*tmp;
	tmp = getenv("WINDOWID");
	if(tmp==NULL)
		return anykey_delete(anykey); 
	/* FIXME if this fails, attempt to grab foregound window */
	anykey->window = strtoul(tmp,NULL,0);

	/* pick a ttyfd in order of preference */
	if(isatty(1)) anykey->tty = 1;
	if(isatty(2)) anykey->tty = 2;
	if(isatty(0)) anykey->tty = 0;

	/* if this process has *no* access to a tty, fail */
	if(anykey->tty == -1)
		return anykey_delete(anykey);
	/* set the tty to not buffer or echo keypresses */
	anykey_charmode(anykey->tty);

	anykey_listen(anykey->display,anykey->window); 

	return anykey;
}
int anykey_canget(struct anykey*anykey)
{
	int err;

	struct pollfd set;
	set.fd = anykey->xfd;
	set.events = POLLIN;
	err = poll(&set,1,0);
	if(err==-1)
		return 0;
	return (set.revents&POLLIN)==POLLIN;
}
char* anykey_getkey(struct anykey*anykey)
{ 
	XEvent event;
	XKeyEvent*kev;
	char* key;

	XNextEvent(anykey->display,&event);
	if(event.type == KeyPress || event.type == KeyRelease)
	{
		kev = (XKeyEvent*)&event;
		key = XKeysymToString(XKeycodeToKeysym(anykey->display, kev->keycode, 0));
		if(key==NULL)
			return NULL;
		
		if(kev->type == KeyPress)
			sprintf(anykey->cache,"KeyDown %s",key);
		else if(kev->type == KeyRelease)
			sprintf(anykey->cache,"KeyUp %s",key);
		else 
			return NULL;
	}

	return anykey->cache;
}

