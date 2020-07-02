#include "vga.h"
#include "filesystem.h"
#include "memmngr.h"

//CRT controller
//Index Register 0x3d4
//Data Register 0x3d5
//Cursor high byte control word is 0xe
//Cursor low byte control word is 0xf

//Choosing start of free memory region (14Mb) in size as screen buffer
#define VGA_BUFFER_START (uint8_t *) 0x100000
#define VGA_BUFFER_END (uint8_t *) 0x105dc0
#define VGA_BUFFER_SIZE (uint32_t) (VGA_BUFFER_END - VGA_BUFFER_START)
#define DISPLAY_START (uint8_t *) (VIDMEM + SCREEN_WIDTH * 0x2 * 0xa)

#define SPLASH_FRAME_HEIGHT 0xa

#define COMMAND_PROMPT_LENGTH 0xb

uint8_t * screen_buffer_pointer = VGA_BUFFER_START;
uint8_t * screen_buffer_end_pointer = VGA_BUFFER_START + DISPLAY_WIDTH * DISPLAY_HEIGHT * 0x2;
uint8_t * screen_display_pointer = DISPLAY_START;
uint8_t * screen_top_limit = VGA_BUFFER_START;

static uint16_t cursor_prompt_location = 0x0;
uint16_t cursor_previous_prompt_location = 0x0;

uint16_t get_cursor () {

        outportb (0x3d4, 0xe);
        uint8_t cursor_high_byte = inportb (0x3d5);
        outportb (0x3d4, 0xf);
        uint8_t cursor_low_byte = inportb (0x3d5);

        uint16_t cursor_position = ((cursor_high_byte << 8) & 0xff00) | cursor_low_byte;
        return cursor_position;

}

void set_cursor (uint16_t position) {

        uint8_t cursor_high_byte = position >> 8;
        uint8_t cursor_low_byte = position & 0xff;

        outportb(0x3d4, 0xe);
        outportb(0x3d5, cursor_high_byte);
        outportb(0x3d4, 0xf);
        outportb(0x3d5, cursor_low_byte);

}

void cursor_enable (bool flag) {

        outportb (0x3d4, 0xa);
        uint8_t cursor_start_register = inportb (0x3d5);
        outportb (0x3d4, 0xa);

        if (flag) {
                outportb (0x3d5, cursor_start_register & ~0x20);
        } else {
                outportb (0x3d5, cursor_start_register | 0x20);
        }
}

bool vga_string_compare (char * vga_string, char * string) {

        while (*vga_string) {
                if (*vga_string != *string)
                        return false;
                vga_string += 0x2;
                string++;
        }

        return (*vga_string == *string);
}

void screen_move_to_cursor (uint16_t * cursor_location, uint16_t * cursor_line) {

        while ((*cursor_line) >= SCREEN_HEIGHT) {
                screen_down ();
                (*cursor_line)--;
                (*cursor_location) -= DISPLAY_WIDTH;
        }

}

void print_char (char c) {

        if (c) {
                uint16_t cursor_location = get_cursor();
                uint16_t cursor_line = (cursor_location + 1) / DISPLAY_WIDTH;
                screen_move_to_cursor (&cursor_location, &cursor_line);
                uint8_t * pixel_location = (uint8_t *) (VIDMEM + cursor_location * 2);
                *pixel_location = c;
                set_cursor (cursor_location + 1);
        }
}

void print_char_color (char c, uint8_t color) {

        if (c) {
                uint16_t cursor_location = get_cursor();
                uint16_t cursor_line = (cursor_location + 1) / DISPLAY_WIDTH;
                screen_move_to_cursor (&cursor_location, &cursor_line);
                uint8_t * pixel_location = (uint8_t *) (VIDMEM + cursor_location * 2);
                *pixel_location = c;
                pixel_location++;
                *pixel_location = color;
                set_cursor (cursor_location + 1);
        }
}

void print_string (char * string) {

        while (*string) {
                print_char (*string);
                string++;
        }

}

void print_string_color (char * string, uint8_t color) {

        while (*string) {
                print_char_color (*string, color);
                string++;
        }

}

void copy_string (uint8_t * source, uint8_t * destination) {

	uint8_t index = 0x0;

	while (source[index]) {

		destination[index] = source[index];
		index++;

	}

	destination[index] = source[index];

}

uint32_t string_len (uint8_t * string) {

	uint32_t length = 0x0;

	while (string[length])
		length++;

	return length;

}

void print_hex (uint32_t value) {

        char * buffer = "0x        ";
        uint8_t mask = 0xf;
        uint8_t count = 0x0;
        uint8_t offset;

        while (count < 0x8) {

                uint8_t value_to_print = (value >> (count * 0x4)) & mask;

                if ( value_to_print > 9 )
                        offset = 0x57;
                else
                        offset = 0x30;

                buffer[0x9 - count] = value_to_print + offset;
                count++;
        }

        print_string (buffer);

}

void print_tab () {

        uint8_t count = 8;

        while (count > 0x0) {
                print_char (' ');
                count--;
        }

}

void print_command_prompt () {

        print_char (' ');
        print_char (0xfe);
        print_string (" mystix:");
        print_string (current_working_dir.name);
        print_string (">");
        cursor_prompt_location = get_cursor ();

}

void print_backspace () {

        uint16_t cursor_location = get_cursor ();
        uint16_t cursor_line = (cursor_location - 1)/ DISPLAY_WIDTH;
        uint16_t cursor_new_location = cursor_location - 1;

        if (cursor_new_location >= cursor_prompt_location) {
                screen_move_to_cursor (&cursor_location, &cursor_line);
                set_cursor (cursor_location - 1);
                uint8_t * pixel_location = (uint8_t *) (VIDMEM + (cursor_location - 1) * 2);
                *pixel_location = 0x0;

        }
}

void screen_save () {

        if (screen_buffer_end_pointer < screen_buffer_pointer) {
                uint32_t split_buffer_one = VGA_BUFFER_END - screen_buffer_pointer;
                uint32_t split_buffer_two = DISPLAY_WIDTH * DISPLAY_HEIGHT * 0x2 - split_buffer_one;
                memcpy (screen_display_pointer, screen_buffer_pointer, split_buffer_one);
                memcpy (screen_display_pointer + split_buffer_one, VGA_BUFFER_START, split_buffer_two);
        } else {
                memcpy (screen_display_pointer, screen_buffer_pointer, DISPLAY_WIDTH * DISPLAY_HEIGHT * 0x2);
        }
}

void screen_refresh () {

        if (screen_buffer_end_pointer < screen_buffer_pointer) {
                uint32_t split_buffer_one = VGA_BUFFER_END - screen_buffer_pointer;
                uint32_t split_buffer_two = DISPLAY_WIDTH * DISPLAY_HEIGHT * 0x2 - split_buffer_one;
                memcpy (screen_buffer_pointer, screen_display_pointer, split_buffer_one);
                memcpy (VGA_BUFFER_START, screen_display_pointer + split_buffer_one, split_buffer_two);

        } else {
                memcpy (screen_buffer_pointer, screen_display_pointer, DISPLAY_WIDTH * DISPLAY_HEIGHT * 0x2);
        }
}

void dec_screen_buffer_pointers () {

        screen_buffer_pointer = VGA_BUFFER_START + (screen_buffer_pointer - DISPLAY_WIDTH * 0x2 - VGA_BUFFER_START + VGA_BUFFER_SIZE) % VGA_BUFFER_SIZE;
        screen_buffer_end_pointer = VGA_BUFFER_START + (screen_buffer_end_pointer - DISPLAY_WIDTH * 0x2 - VGA_BUFFER_START + VGA_BUFFER_SIZE) % VGA_BUFFER_SIZE;

}

void inc_screen_buffer_pointers () {

        screen_buffer_pointer = VGA_BUFFER_START + (screen_buffer_pointer + DISPLAY_WIDTH * 0x2 - VGA_BUFFER_START) % VGA_BUFFER_SIZE;
        screen_buffer_end_pointer = VGA_BUFFER_START + (screen_buffer_end_pointer + DISPLAY_WIDTH * 0x2 - VGA_BUFFER_START) % VGA_BUFFER_SIZE;

        if (screen_buffer_end_pointer == screen_top_limit) {
                screen_top_limit = VGA_BUFFER_START + (screen_top_limit + DISPLAY_WIDTH * DISPLAY_HEIGHT * 0x2 - VGA_BUFFER_START) % VGA_BUFFER_SIZE ;
                memset (0x1f00, (uint16_t *) screen_buffer_end_pointer, DISPLAY_WIDTH * DISPLAY_HEIGHT);
        }

}

void screen_up () {

        bool condition = true;

        if (screen_buffer_pointer >= screen_top_limit)
                condition = (screen_buffer_pointer - DISPLAY_WIDTH * 0x2) >= screen_top_limit;

        if (condition) {
                screen_save ();
                dec_screen_buffer_pointers ();
                screen_refresh ();
                cursor_prompt_location += DISPLAY_WIDTH;
                set_cursor ( get_cursor () + DISPLAY_WIDTH );
        }

}

void screen_down () {

        bool condition = cursor_prompt_location /DISPLAY_WIDTH > SPLASH_FRAME_HEIGHT;

        if (condition) {
                screen_save ();
                inc_screen_buffer_pointers ();
                screen_refresh ();
                cursor_prompt_location -= DISPLAY_WIDTH;
                set_cursor ( get_cursor () - DISPLAY_WIDTH );
        }

}

void print_new_line () {

        uint16_t cursor_new_location = ( (get_cursor () / SCREEN_WIDTH) + 1 ) * SCREEN_WIDTH;

        while (cursor_new_location >= (SCREEN_WIDTH * SCREEN_HEIGHT)) {

                screen_down ();
                cursor_new_location -= SCREEN_WIDTH;

        }

        set_cursor (cursor_new_location);
        cursor_previous_prompt_location = cursor_prompt_location;
        cursor_prompt_location = cursor_new_location;

}

void copy_argv (uint8_t * argv, uint8_t * buffer) {

        while ( *argv != 0x0 ) {
                *buffer = *argv;
                argv += 2;
                buffer ++;
        }

}

void program_clear () {

        uint16_t cursor_line = cursor_prompt_location / DISPLAY_WIDTH;

        while (cursor_line >= SPLASH_FRAME_HEIGHT) {
                screen_down();
                cursor_line--;
        }

}

void normalize_filesystem_address (uint8_t * filesystem_address, uint8_t * buffer) {

        uint8_t index = 0x0;
        uint8_t offset = 0x0;

        while (filesystem_address[index+offset]) {
                buffer[index] = filesystem_address[index+offset];
                index++;
                if (filesystem_address[index+offset-1] == '/') {
                        while (filesystem_address[index+offset] == '/')
                                offset++;
                }
        }

        buffer[index] = 0x0;

}

void program_ls (uint8_t * argv) {

	uint8_t buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];
        memset (0x0, (uint16_t *) buffer, sizeof(buffer) / 0x2);
	normalize_filesystem_address (argv, buffer);

	struct dir_entry _dir_entry = filesystem_search ( buffer );
        uint32_t file_inode = _dir_entry.inode;

        if ( file_inode == 0x0 ) {
                print_string ("No such file or directory: ");
                print_string (buffer);
        } else if ( file_inode == 0x1) {
                print_string ("Not a directory: ");
                print_string (buffer);
        } else if ( _dir_entry.type_or_name_length_high == DE_DIR ) {
                print_dir (_dir_entry.inode);
        } else {
                print_string ("The file inode is: ");
                print_hex (file_inode);
        }

        print_new_line ();
}

void program_cd (uint8_t * argv) {

	uint8_t buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];
        memset (0x0, (uint16_t *) buffer, sizeof(buffer) / 0x2);
	normalize_filesystem_address (argv, buffer);

	struct dir_entry _dir_entry = filesystem_search ( buffer );
        uint32_t file_inode = _dir_entry.inode;

        if ( file_inode == 0x0 ) {
                print_string ("No such file or directory: ");
                print_string (buffer);
        } else if ( file_inode == 0x1 || (_dir_entry.type_or_name_length_high != DE_DIR) ) {
                print_string ("Not a directory: ");
                print_string (buffer);
        } else {
                change_dir (_dir_entry, buffer);
        }

        print_new_line ();

}

void program_mkdir (uint8_t * address) {

//TODO check address before making call to filesystem driver

	mkdir (address);

}

void program_rmdir (uint8_t * address) {

//TODO check address before making call to filesystem driver

	rmdir (address);

}

void parse_command_line (uint8_t * command_line, uint32_t parsed_command_line[2]) {

        parsed_command_line[0] = (uint32_t) command_line;

        while ( (*command_line != ' ') && (*command_line != 0x0) )
                command_line +=2;

        *command_line = 0x0;
        command_line += 2;

        parsed_command_line[1] = (uint32_t) command_line;

}

void parse_command () {

        uint8_t * command_line = (uint8_t *) (VIDMEM + (cursor_previous_prompt_location) * 0x2);

        uint32_t parsed_command_line[2];

        parse_command_line (command_line, parsed_command_line);

        uint8_t * command = (uint8_t *) (parsed_command_line[0]);
        uint8_t * argv = (uint8_t *) (parsed_command_line[1]);

        uint8_t buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];
        memset (0x0, (uint16_t *) buffer, sizeof(buffer) / 0x2);

	copy_argv (argv, buffer);

        if (vga_string_compare (command, "clear")) {
                program_clear ();
        } else if (vga_string_compare (command, "ls")) {
                program_ls ( buffer );
        } else if (vga_string_compare (command, "cd")) {
                program_cd ( buffer );
        } else if (vga_string_compare (command, "mkdir")) {
		program_mkdir ( buffer );
	} else if (vga_string_compare (command, "rmdir")) {
		program_rmdir ( buffer );
	} else if (vga_string_compare (command, "")) {
        } else {
                print_string ("command not found");
                print_new_line ();
        }

}

void screen_initialize () {

        print_command_prompt ();

        cursor_prompt_location = get_cursor ();

        //set buffer space to 0 character and blue background two bytes at a time
        memset (0x1f00, (uint16_t *) VGA_BUFFER_START, (uint32_t) (VGA_BUFFER_END - VGA_BUFFER_START) / 0x2);
}

