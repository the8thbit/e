#ifndef CODES_H
#define CODES_H

#include <termios.h> /* struct termios */

/** ASCII definitions **/
#define K_ENT 10
#define K_ESC 27
#define K_BKS 127
#define CONTROL(c) (c-96)
#define SHIFT(c) (c-32)


/** Terminal operations **/
typedef struct termios tstate;
int t_getstate(tstate *state); /* get current state */
tstate t_initstate(const tstate *state); /* take state, set needed flags and return */
int t_setstate(const tstate *state); /* set state */
int t_clear(); /* send clear code */
int t_getwidth(); /* get terminal width */
int t_getheight(); /* get terminal height */
int t_read(char *c, size_t len); /* request a read of up to len chars into c, will memset c to 0 first */

#endif
