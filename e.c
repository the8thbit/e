#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "codes.h"

static tstate original, n_state; /* states of the terminal */


unsigned int read_file( char file_name[], char** edit_buffer, register unsigned int* fail ) {
  FILE* file_pointer;
  DIR*  dir_pointer;
  char file_path[strlen( file_name )];
  register unsigned int file_size;
  register unsigned int file_name_pos = strlen( file_name );
 
  strcpy( file_path, file_name );

  if( ( file_pointer = fopen( file_path, "rb" ) ) != NULL ) {
    if( access( file_path, W_OK ) ) {
      if( file_path[file_name_pos-1] != '/' ) {
        fseek( file_pointer, 0L, SEEK_END );
        file_size = ftell( file_pointer );
        fseek( file_pointer, 0L, SEEK_SET );

        if( file_size < 64 ) { file_size = 64; }

        if( ( *edit_buffer = malloc( file_size * 2 ) ) != NULL ) {
          fread( *edit_buffer, file_size, 1, file_pointer );
        } else { *fail = 1; }
      } else { *fail = 1; }
    } else { *fail = 1; }
    
    fclose( file_pointer );
  } else {
    file_size = 64;

    while( 1 ) {
      if( file_path[file_name_pos] == '/' ) {
        if( file_name_pos < strlen( file_path ) ) {
          file_path[file_name_pos+1] = '\0';
        } else { file_path[file_name_pos] = '\0'; }
        break; 
      }
      if( file_name_pos == 0 ) { break; }
      file_name_pos--;
    }

    if( access( file_path, W_OK ) == 0 ) {
      if( file_name_pos == 0 && file_path[0] != '/' ) {
         if( ( *edit_buffer = malloc( file_size * 2 ) ) != NULL ) {
           strcpy( *edit_buffer, "" );
           if( access( file_path, R_OK ) != 0 || access( file_path, W_OK ) != 0  ) { *fail = 1; }
         } else { *fail = 1; }
      } else 
      if( ( *edit_buffer = malloc( file_size * 2 ) ) != NULL
      &&  ( dir_pointer = opendir( file_path ) ) != NULL ) {
        strcpy( *edit_buffer, "" );
        closedir( dir_pointer );
        if( access( file_path, R_OK ) != 0 || access( file_path, W_OK ) != 0 ) { *fail = 1; }
      } else { *fail = 1; }
    } else { *fail = 1; }
  }
  
  return file_size * 2;
}

void save_file( char file_name[], char** edit_buffer, int* fail ) {
  FILE* file_pointer;
  register unsigned int file_size;

  puts( file_name );

  if( ( file_pointer = fopen( file_name, "w" ) ) != NULL ) {
    fputs( *edit_buffer, file_pointer );
    fclose ( file_pointer );
  } else { *fail = 1; }
}

unsigned int add_char( char new_char, register unsigned int pos, char** edit_buffer, char** temp_buffer, register unsigned int alloc_size, register unsigned int* fail ) { 
 if( strlen( *edit_buffer ) + 1 >= alloc_size ) {
    char* realloc_buffer;
    alloc_size += alloc_size;
    *edit_buffer = ( realloc_buffer = realloc( *edit_buffer, alloc_size ) );
    *temp_buffer = ( realloc_buffer = realloc( *temp_buffer, alloc_size ) );
  }

  if( *edit_buffer != NULL && *temp_buffer != NULL ) {
    strcpy( *temp_buffer, *edit_buffer + pos );
    strncat( *temp_buffer, *edit_buffer, pos );
    strcpy( *edit_buffer, *temp_buffer + ( strlen( *temp_buffer ) - pos ) );
    strncat( *edit_buffer, &new_char, 1 );
    strncat( *edit_buffer, *temp_buffer, ( strlen( *temp_buffer ) - pos ) );
  } else { fail = 0; }

  return alloc_size;
}

unsigned int remove_char( register unsigned int pos, char** edit_buffer, char** temp_buffer, register unsigned int alloc_size, register unsigned int* fail ) {
  if( strlen( *edit_buffer ) * 4 <= alloc_size ) {
    char* realloc_buffer;
    alloc_size /= 2;
    *edit_buffer = ( realloc_buffer = realloc( *edit_buffer, alloc_size ) );
    *temp_buffer = ( realloc_buffer = realloc( *temp_buffer, alloc_size ) );
  }

  if( *edit_buffer != NULL && *temp_buffer != NULL ) {
    strcpy( *temp_buffer, *edit_buffer + pos );
    strncat( *temp_buffer, *edit_buffer, pos - 1 );
    strcpy( *edit_buffer, *temp_buffer + ( strlen( *temp_buffer ) - ( pos - 1 ) ) );
    strncat( *edit_buffer, *temp_buffer, ( strlen( *temp_buffer ) - pos + 1 ) );
  } else { *fail = 0; }

  return alloc_size;
}

unsigned int main( register unsigned int argc, char* argv[] ) {
  unsigned int fail = 0; /* success and golf are scored in the same way */
  register unsigned int pos = 0;
  register unsigned int alloc_size;
  char ch[7];
  char* edit_buffer;
  char* temp_buffer;

  if( argc != 2 ) {
    fail = 1;
  } else {
    t_getstate( &original );
    n_state = t_initstate( &original );
    t_setstate( &n_state );
    alloc_size = read_file( argv[1], &edit_buffer, &fail );
    
    if( !fail) { temp_buffer = malloc( alloc_size ); }

    while( !fail ) {
      //puts( edit_buffer );
      t_read( ch, 7 );
      /* commands */
      /* cursor movement */
      if( ch[0] == 27 && ch[1] == 'j' && ch[2] == 0 ) { /* alt+j - move backwards */
        if( pos > 0 ) { pos -= 1; }
      } else
  
      if( ch[0] == 27 && ch[1] == 'k' && ch[2] == 0 ) { /* alt+k - move forwards */
        if( pos < strlen( edit_buffer ) ) { pos += 1; }
      } else
  
      /* other commands */  
      if( ch[0] == 27 && ch[1] == 'd' && ch[2] == 0 ) { /* alt+d - save file */
        save_file( argv[1], &edit_buffer, &fail );  
      } else

      if( ch[0] == 27 && ch[1] == 'f' && ch[2] == 0 ) { /* alt+f - quit program. Note: Don't hit this by accident. */
        break;
      } else

      /* non-chorded keys */
      if( ch[0] == 127 && ch[1] == 0 && ch[2] == 0 ) { /* backspace - delete previous character */
        if( pos > 0 ) {
          alloc_size = remove_char( pos, &edit_buffer, &temp_buffer, alloc_size, &fail );
          pos--;
        }
      } else
      
      if( ch[0] != 0 && ch[1] == 0 && ch[2] == 0 ) { /* add any other character pressed */
        alloc_size = add_char( ch[0], pos, &edit_buffer, &temp_buffer, alloc_size, &fail );
        pos++;
      }
    }

		
    if( alloc_size > 0 && !fail ) {
      free( edit_buffer );
      free( temp_buffer );
    }
    t_setstate( &original );
    fflush( stdout );
  }
  
  return fail;
};
