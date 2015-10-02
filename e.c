/******************************************************************************
 * e: The minimalistest text editor
 *
 * Author: Peter Sussman
 * Description: This is the most important file, I guess...
 * Copyright: Copyright (C) 2012-2013 Peter Sussman
 * License: GNU AGPL version 3.0+
 * ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "codes.h"

static tstate original, n_state; /* states of the terminal */

unsigned int read_file(char** edit_buffer, register unsigned int *fails) {
	register size_t file_size;
	register size_t data_read = 0;
	*edit_buffer = malloc(0);

	while(!ferror(stdin) && !feof(stdin)) {
		*edit_buffer = realloc(*edit_buffer, 4096+data_read);
		if(*edit_buffer == NULL) {
			*fails = 1;
			return 0;
		}
		data_read += fread(*edit_buffer, 1, 4096, stdin);
	}

	if (data_read == 0) {
		*edit_buffer = malloc(4096);
		if(*edit_buffer == NULL) {
			*fails = 1;
			return 0;
		}
	}
	return data_read; 
}

unsigned int add_char(char new_char, register unsigned int pos, char** edit_buffer, char** temp_buffer, register unsigned int alloc_size, register unsigned int* fail) { 
	if (strlen( *edit_buffer ) + 1 >= alloc_size) {
		char* realloc_buffer;
		alloc_size += alloc_size;
		*edit_buffer = (realloc_buffer = realloc(*edit_buffer, alloc_size));
		*temp_buffer = (realloc_buffer = realloc(*temp_buffer, alloc_size));
	}

	if (*edit_buffer != NULL && *temp_buffer != NULL) {
		strcpy(*temp_buffer, *edit_buffer + pos);
		strncat(*temp_buffer, *edit_buffer, pos);
		strcpy(*edit_buffer, *temp_buffer + (strlen(*temp_buffer) - pos));
		strncat(*edit_buffer, &new_char, 1);
		strncat(*edit_buffer, *temp_buffer, (strlen(*temp_buffer) - pos));
	} else {
		fail = 0;
	}

	return alloc_size;
}

unsigned int remove_char(register unsigned int pos, char** edit_buffer, char** temp_buffer, register unsigned int alloc_size, register unsigned int* fail) {
	if (strlen(*edit_buffer) * 4 <= alloc_size) {
		char* realloc_buffer;
		alloc_size /= 2;
		*edit_buffer = (realloc_buffer = realloc(*edit_buffer, alloc_size));
		*temp_buffer = (realloc_buffer = realloc(*temp_buffer, alloc_size));
	}

	if(*edit_buffer != NULL && *temp_buffer != NULL) {
		strcpy(*temp_buffer, *edit_buffer + pos);
		strncat(*temp_buffer, *edit_buffer, pos - 1);
		strcpy(*edit_buffer, *temp_buffer + (strlen(*temp_buffer) - (pos - 1)));
		strncat(*edit_buffer, *temp_buffer, (strlen(*temp_buffer) - (pos + 1)));
	} else {
		*fail = 0;
	}

	return alloc_size;
}

int main(register int argc, char* argv[]) {
	unsigned int fail = 0; /* success and golf are scored in the same way */
	register unsigned int pos = 0;
	register unsigned int alloc_size;
	char ch[7];
	char* edit_buffer;
	char* temp_buffer;

	t_getstate(&original);
	n_state = t_initstate(&original);
	t_setstate(&n_state);
	alloc_size = read_file(&edit_buffer, &fail);

	if (!fail) {
		temp_buffer = malloc(alloc_size);
	}

	while(!fail) {
		t_read(ch, 7);
		/* commands */
		if (ch[0] == 27 && ch[1] == 'j' && ch[2] == 0) { /* alt+j - move backwards */
			if(pos > 0) {
				pos -= 1;
			}
		} else if (ch[0] == 27 && ch[1] == 'k' && ch[2] == 0) { /* alt+k - move forwards */
			if (pos < strlen(edit_buffer)) {
				pos += 1;
			}
		} else if (ch[0] == 27 && ch[1] == 'd' && ch[2] == 0) { /* alt+d - save file */
			fputs(edit_buffer, stdout);
		} else if (ch[0] == 27 && ch[1] == 'f' && ch[2] == 0) { /* alt+f - quit program. Note: Don't hit this by accident. */
			break;
		} else if (ch[0] == 127 && ch[1] == 0 && ch[2] == 0) { /* backspace - delete previous character */
			if (pos > 0) {
				alloc_size = remove_char(pos, &edit_buffer, &temp_buffer, alloc_size, &fail);
				pos--;
			}
		} else if(ch[0] != 0 && ch[1] == 0 && ch[2] == 0) { /* add any other character pressed */
			alloc_size = add_char(ch[0], pos, &edit_buffer, &temp_buffer, alloc_size, &fail);
			pos++;
		}
	}

	if (alloc_size > 0 && !fail) {
		free(edit_buffer);
		free(temp_buffer);
	}
	t_setstate(&original);
	fflush(stdout);

	return fail;
}
