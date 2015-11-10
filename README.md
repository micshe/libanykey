#libanykey
uses a trick to let terminal apps read *any* key or *any* key combination from the keyboard.  depends on libX11.

#how it works
by convention (and convention alone) software terminals in X tend to define an environment variable $WINDOWID, that holds the X11 window id of the terminal.  any process started from within this software terminal gets access to the $WINDOWID.  these programs can then use libX11 or libxcb to interact with the software terminal's window in the usual way.  libanykey uses libX11 (link with -lX11) internally.  it may switch to libxcb.

of course, this means you can use libanykey to spy on the keypresses to another window, just by changing the value of $WINDOWID.  this is particularly obvious when using terminator, where all tiles share the same $WINDOWID.  two libanykey apps runnning in two side-by-side tiles can't help but evesdrop on each other.

libanykey takes care of hiding the X11 connection, and draining the keypresses from stdin.  to keep things predictable, if stdin,stdout,stderr are all redirected to files, then libanykey won't read any keypresses.  this keeps it consistent with tools like less.

#co-operating software terminals
- xterm
- rxvt
- konsole
- suckless-terminal (st)
- terminator (GNOME-2 VTE widget based)
- xfce4-term (GNOME VTE widget based)
- terminolgoy (enlightenment terminal)

#unco-operating software terminals
- LXterminal (GNOME-2 VTE widget based) (does not set $WINDOWID)
- gnome-terminal (GNOME VTE widget based) (sets $WINDOWID, but not reporting events [similar to konsole under xcb])

#untested terminals
- guake (GNOME VTE widget based)

#build
 todo

#documentation
 todo

#examples
 todo

#todo
 - add a best-guess fallback incase the terminal doesn't define $WINDOWID (grab whatever window is in focus?)
 - check if the app is in the terminals foreground before reading a key, otherwise background jobs will still receive input. (getsid()==getpgid())
 - add support for reading mouse events (simple)
 - add support for the kernel's virtual terminals via libinput or systemd (probably difficult)
 - move to libxcb (in-progress, doesn't work for konsole for unknown reasons)

#other applications of the $WINDOWID trick
in theory the same trick can be used to draw full-color pixel images *into* the terminal window.  in practice, this only seems to work with xterm.  Konsole and GNOME-VTE widget based terminals don't seem to allow drawing to the windows.

a work-around would be to temporarily reparent or place a drawable window into or ontop of the software terminal's window.  this would be more like a screen-mode then live graphics though, and would require more of a support framework.

might give it a shot in future.  it'd be useful for mucking around with.

#OSX support
- unknown.  i'm sure there's a similar trick can could be pulled in OSX, but i've no idea what it is.

#Wayland support
- unknown.  again, i'm sure a similar trick could be performed, but haven't looked into it yet.

#readanykey tool
simple libanykey tool, for use in shellscripts or via a popen() command.

#sh example
 todo

#python subprocess example
 todo

#C popen() example
 todo

