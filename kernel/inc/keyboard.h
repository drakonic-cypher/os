#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "system.h"
#include "idt.h"
#include "vga.h"

void handler_keyboard (struct regs * r);
//void keyboard_initialize ();

#endif
