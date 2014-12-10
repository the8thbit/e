/*************************************************************************\
| Title:       codes.c
| Author:      Peter Sussman
| Description: Modified from original codes.c by Chris Hall. A lightweight
|              interface for chorded interaction directly through a UNIX
|              terminal.
\*************************************************************************/

#include <unistd.h> /* tcgetattr and tcsetattr */
#include <stdio.h>
#include <sys/ioctl.h> /* TCIOCGWINSZ */
#include <string.h> /* strlen */
#include "codes.h"

/** Terminal operations **/
int t_getstate(tstate *state){
	if (tcgetattr(1, state)) {
		return -1;
	}
	return 0;
}

tstate t_initstate(const tstate *state){
	tstate nstate      = *state;
	nstate.c_lflag     = 0;     /* set non-canonical */
	nstate.c_cc[VTIME] = 0;     /* set timeout */
	nstate.c_cc[VMIN]  = 1;     /* set read to return after it has one char available */
	return nstate;
}

int t_setstate(const tstate *state){
	if (tcsetattr(1, TCSANOW, state)) {
		return -1;
	}
	return 0;
}

int t_clear() {
	return fputs("[2J", stdout );
}

int t_getwidth() {
	/* do I need to take TCIOCGSIZE into account? if so http://www.linuxquestions.org/questions/programming-9/get-width-height-of-a-terminal-window-in-c-810739/ */
	struct winsize ts;
	if (ioctl(0, TIOCGWINSZ, &ts)) {
		return -1;
	}
	return ts.ws_col;
}

int t_getheight() {
	/* do I need to take TCIOCGSIZE into account? if so http://www.linuxquestions.org/questions/programming-9/get-width-height-of-a-terminal-window-in-c-810739/ */
	struct winsize ts;
	if(ioctl(0, TIOCGWINSZ, &ts)) {
		return -1;
	}
	return ts.ws_row;
}

int t_read(char *c, size_t len) {
	memset(c, 0, len*sizeof(char));
	return read(0, c, len);
}
