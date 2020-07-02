#ifndef _VGA_H
#define _VGA_H

#include "system.h"

#define SCREEN_WIDTH 0x50
#define SCREEN_HEIGHT 0x19

void print_char (char c);
void print_char_color (char c, uint8_t color);
void print_string (char * string);
void print_string_color (char * string, uint8_t color);
void copy_string (uint8_t * source, uint8_t * destination);
uint32_t string_len (uint8_t * string);
void print_hex (uint32_t value);
void print_tab ();
void print_command_prompt ();
void print_new_line ();
void print_backspace ();
void screen_up ();
void screen_down ();
void parse_command ();
void screen_initialize ();


#endif
