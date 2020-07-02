#include "timer.h"

//port 0x43 is the command register for the PIT
//port 0x40-0x42 are the data registers for the three channels of the PIT
//the 0th and the 2nd channel are the most important (the second is connected to the speaker)
//need to send the LSB (lowest significant byte) followed by the MSB (most significant byte)
//of the value for the counter frequency (value by which 1193182Hz will be divided by)

#define FRAME_COMPONENT 0xba
#define FRAME_START ((uint8_t *) (VIDMEM + 0xa0 + 0x6)) //Y_OFFSET + X_OFFSET
#define FRAME_WIDTH 0x49
#define FRAME_HEIGHT 0x7
#define FRAME_END ((uint8_t *) (FRAME_START + FRAME_WIDTH * 0x2 * FRAME_HEIGHT))

uint32_t timer_global_count = 0;
uint8_t * timer_frame_component_addr = FRAME_START;

//channel is either 0 or 2
void timer_set_frequency (uint8_t channel, uint16_t frequency) {

	outportb (0x43, 0x36 | (channel << 6) );
	uint16_t frequency_divisor = (uint32_t) 0x1234de / frequency;
	outportb (0x40 | channel, frequency_divisor & 0xff);
	outportb (0x40 | channel, frequency_divisor >> 8);

}

// the speaker is located at port 0x61, must be turned on by bits 0 and 1
void speaker_on () {

	uint8_t speaker_status = inportb (0x61);
        outportb (0x61, speaker_status | 0x3);

}

void speaker_off () {

 	uint8_t speaker_status = inportb (0x61);
        outportb (0x61, speaker_status & 0xfc);
}

//100 microseconds * 10 is 1 millisecond
void timer_wait (uint16_t milli_seconds) {

	uint16_t micro_seconds = milli_seconds * 10;

	//__asm__ ("cli");
	timer_global_count = 0;
	//__asm__ ("sti");

	while ( timer_global_count < micro_seconds ) {

	}
}

void timer_setup () {

        //100 microsecond intervals
        timer_set_frequency (0x0, 0x2710);

        //setup beep (channel 2) frequency
        timer_set_frequency (0x2, 261);
}


void play_sound (uint16_t frequency) {

	timer_set_frequency (0x2, frequency);
	speaker_on();
	timer_wait (100);
	speaker_off();

}

void timer_animate_frame () {

	int16_t frame_delta = 0xa0;

	while (0x1) {

		*timer_frame_component_addr = FRAME_COMPONENT;
		timer_wait (250);
		*timer_frame_component_addr = ' ';

		if ((timer_frame_component_addr + frame_delta) > FRAME_END) {
			timer_frame_component_addr += (FRAME_WIDTH * 0x2);
			frame_delta = -0xa0;
		} else if ( (timer_frame_component_addr + frame_delta) < FRAME_START ) {
			timer_frame_component_addr -= (FRAME_WIDTH * 0x2);
			frame_delta = 0xa0;
		} else {
			timer_frame_component_addr += frame_delta;
		}
	}

}

void handler_timer (struct regs * r) {

	timer_global_count++;

}
