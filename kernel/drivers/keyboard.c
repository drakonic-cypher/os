#include "keyboard.h"

//The keyboard controller has three 8-bit registers involved with the CPU:
//Input buffer, that can be written by the CPU at port 0x60 or port 0x64;
//Output buffer, that can be read by the CPU by reading from port 0x60
//Status register, that can be read by the CPU by reading from port 0x64.

/* US Keyboard Layout. Scancode table used to layout a standard US keyboard. */

uint8_t keyboard_us_lowercase [128] = {

	0,  27, '1', '2', '3', '4', '5', '6', '7', '8',		/* 9 */
  	'9', '0', '-', '=', '\b',				/* Backspace */
  	'\t',							/* Tab */
  	'q', 'w', 'e', 'r',					/* 19 */
  	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    	0,							/* 29   - Control */
  	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 	'\'', '`',   0,						/* Left shift */
 	'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  	'm', ',', '.', '/',   0,				/* Right shift */
  	'*',
    	0,							/* Alt */
  	' ',							/* Space bar */
    	0,							/* Caps lock */
    	0,							/* 59 - F1 key ... > */
    	0,   0,   0,   0,   0,   0,   0,   0,
    	0,							/* < ... F10 */
    	0,							/* 69 - Num lock*/
    	0,							/* Scroll Lock */
    	0,							/* Home key */
    	0,							/* Up Arrow */
    	0,							/* Page Up */
  	'-',
    	0,							/* Left Arrow */
    	0,
    	0,							/* Right Arrow */
  	'+',
    	0,							/* 79 - End key*/
    	0,							/* Down Arrow */
    	0,							/* Page Down */
    	0,							/* Insert Key */
    	0,							/* Delete Key */
    	0,   0,   0,
    	0,							/* F11 Key */
    	0,							/* F12 Key */
    	0,							/* All other keys are undefined */
};

uint8_t keyboard_us_uppercase [128] = {

        0,  27, '1', '2', '3', '4', '5', '6', '7', '8',         /* 9 */
        '9', '0', '-', '=', '\b',                               /* Backspace */
        '\t',                                                   /* Tab */
        'Q', 'W', 'E', 'R',                                     /* 19 */
        'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',           /* Enter key */
        0,                                                      /* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',       /* 39 */
        '\'', '`',   0,                                         /* Left shift */
        '\\', 'Z', 'X', 'C', 'V', 'B', 'N',                     /* 49 */
        'M', ',', '.', '/',   0,                                /* Right shift */
        '*',
        0,                                                      /* Alt */
        ' ',                                                    /* Space bar */
        0,                                                      /* Caps lock */
        0,                                                      /* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,                                                      /* < ... F10 */
        0,                                                      /* 69 - Num lock*/
        0,                                                      /* Scroll Lock */
        0,                                                      /* Home key */
        0,                                                      /* Up Arrow */
        0,                                                      /* Page Up */
        '-',
        0,                                                      /* Left Arrow */
        0,
        0,                                                      /* Right Arrow */
        '+',
        0,                                                      /* 79 - End key*/
        0,                                                      /* Down Arrow */
        0,                                                      /* Page Down */
        0,                                                      /* Insert Key */
        0,                                                      /* Delete Key */
        0,   0,   0,
        0,                                                      /* F11 Key */
        0,                                                      /* F12 Key */
        0,                                                      /* All other keys are undefined */
};

uint8_t keyboard_us_shift_lowercase [128] = {

        0,  27, '!', '@', '#', '$', '%', '^', '&', '*',         /* 9 */
        '(', ')', '_', '+', '\b',                               /* Backspace */
        '\t',                                                   /* Tab */
        'q', 'w', 'e', 'r',                                     /* 19 */
        't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',           /* Enter key */
        0,                                                      /* 29   - Control */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',       /* 39 */
        '"', '~',   0,                                         /* Left shift */
        '|', 'z', 'x', 'c', 'v', 'b', 'n',                     /* 49 */
        'm', '<', '>', '?',   0,                                /* Right shift */
        '*',
        0,                                                      /* Alt */
        ' ',                                                    /* Space bar */
        0,                                                      /* Caps lock */
        0,                                                      /* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,                                                      /* < ... F10 */
        0,                                                      /* 69 - Num lock*/
        0,                                                      /* Scroll Lock */
        0,                                                      /* Home key */
        0,                                                      /* Up Arrow */
        0,                                                      /* Page Up */
        '-',
        0,                                                      /* Left Arrow */
        0,
        0,                                                      /* Right Arrow */
        '+',
        0,                                                      /* 79 - End key*/
        0,                                                      /* Down Arrow */
        0,                                                      /* Page Down */
        0,                                                      /* Insert Key */
        0,                                                      /* Delete Key */
        0,   0,   0,
        0,                                                      /* F11 Key */
        0,                                                      /* F12 Key */
        0,                                                      /* All other keys are undefined */
};

uint8_t keyboard_us_shift_uppercase [128] = {

        0,  27, '!', '@', '#', '$', '%', '^', '&', '*',         /* 9 */
        '(', ')', '_', '+', '\b',                               /* Backspace */
        '\t',                                                   /* Tab */
        'Q', 'W', 'E', 'R',                                     /* 19 */
        'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',           /* Enter key */
        0,                                                      /* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',       /* 39 */
        '"', '~',   0,                                         /* Left shift */
        '|', 'Z', 'X', 'C', 'V', 'B', 'N',                     /* 49 */
        'M', '<', '>', '?',   0,                                /* Right shift */
        '*',
        0,                                                      /* Alt */
        ' ',                                                    /* Space bar */
        0,                                                      /* Caps lock */
        0,                                                      /* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,                                                      /* < ... F10 */
        0,                                                      /* 69 - Num lock*/
        0,                                                      /* Scroll Lock */
        0,                                                      /* Home key */
        0,                                                      /* Up Arrow */
        0,                                                      /* Page Up */
        '-',
        0,                                                      /* Left Arrow */
        0,
        0,                                                      /* Right Arrow */
        '+',
        0,                                                      /* 79 - End key*/
        0,                                                      /* Down Arrow */
        0,                                                      /* Page Down */
        0,                                                      /* Insert Key */
        0,                                                      /* Delete Key */
        0,   0,   0,
        0,                                                      /* F11 Key */
        0,                                                      /* F12 Key */
        0,                                                      /* All other keys are undefined */
};


uint8_t * keyboard_default = keyboard_us_lowercase;

void wait_input () {

	while ( inportb (0x64) & 0x2 ) {

	}

}

void wait_output () {

	while ( !(inportb (0x64) & 0x1 ) ) {

	}

}

uint8_t get_key_press (uint8_t index) {

	return keyboard_default[index];
}

void keyboard_caps_switch () {

	if (keyboard_default == keyboard_us_lowercase)
        	keyboard_default = keyboard_us_uppercase;
        else
        	keyboard_default = keyboard_us_lowercase;

}

void keyboard_shift_switch () {

	if (keyboard_default == keyboard_us_lowercase)
                keyboard_default = keyboard_us_shift_uppercase;
        else if (keyboard_default == keyboard_us_uppercase)
                keyboard_default = keyboard_us_shift_lowercase;
	else if (keyboard_default == keyboard_us_shift_uppercase)
		keyboard_default = keyboard_us_lowercase;
	else
		keyboard_default = keyboard_us_uppercase;
}

void keyboard_backspace_function () {

	print_backspace ();

}

void keyboard_tab_function () {

	print_tab ();

}

void keyboard_enter_function () {

	print_new_line ();
	parse_command ();
	print_command_prompt ();

}

void keyboard_up_arrow_function () {

	screen_up ();

}

void keyboard_down_arrow_function () {

	screen_down ();

}

void keyboard_reset_function () {

	outportb (0x64, 0xfe);

}

void handler_keyboard (struct regs * r) {

	//check if key was presssed or released
	//released if it is greater than 0x80
	uint8_t keyboard_value = inportb (0x60);
	bool pressed = keyboard_value <= 0x80;

	if (keyboard_value == 0xe) {						//value of backspace

		keyboard_backspace_function ();

	} else if (keyboard_value == 0xf) {					//value of tab
		keyboard_tab_function ();
	} else if (keyboard_value == 0x1c) {					//value of enter

		keyboard_enter_function ();

	} else if (keyboard_value == 0x48) {					//value of up-arrow

		keyboard_up_arrow_function ();

	} else if (keyboard_value == 0x50) {					//value of down-arrow

		keyboard_down_arrow_function ();

	} else if (keyboard_value == 0x1) {

		keyboard_reset_function ();

	} else if (pressed) {

		if ( (keyboard_value == 0x2a) | (keyboard_value == 0x36) )	//value of r-shift, l-shift
			keyboard_shift_switch();

		print_char( get_key_press(keyboard_value) );

	} else if (keyboard_value == 0xba) {					//value of caps-lock release

		keyboard_caps_switch();

	} else if ( (keyboard_value == 0xaa) | (keyboard_value == 0xb6) ) {	//value of r-shift, l-shift release

		keyboard_shift_switch();

	}

}
