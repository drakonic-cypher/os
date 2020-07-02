#include "system.h"

// Global Tick count
uint32_t                pit_ticks       =       0;

// send command to pic
void i86_pit_send_command (uint8_t cmd) {

	outportb (I86_PIT_REG_COMMAND, cmd);
}

// send data to a counter
void i86_pit_send_data (uint16_t data, uint8_t counter) {

	uint8_t	port= (counter==I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
		((counter==I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

	outportb (port, data);
}

// read data from counter
uint8_t i86_pit_read_data (uint16_t counter) {

	uint8_t	port= (counter==I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
		((counter==I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

	return inportb (port);
}

void i86_pit_start_counter (uint32_t freq, uint8_t counter, uint8_t mode) {

	if (freq==0)
		return;

	uint16_t divisor = 1193180 / freq;

	// send operational command
	uint8_t ocw=0;
	ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
	ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
	i86_pit_send_command (ocw);

	// set frequency rate
	i86_pit_send_data (divisor & 0xff, 0);
	i86_pit_send_data ((divisor >> 8) & 0xff, 0);

	// reset tick count
	pit_ticks=0;
}
