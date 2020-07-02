#include "system.h"

//inline because they are small routines, this takes out function call
//must be static so that function is provided to linker due to compiler optimization
static inline void i86_pic_send_command (uint8_t cmd, uint8_t pic_num) {

	if (pic_num > 1)
		return;

	uint8_t	reg = (pic_num == 1) ? I86_PIC2_REG_COMMAND : I86_PIC1_REG_COMMAND;
	outportb (reg, cmd);
}

static inline void i86_pic_send_data (uint8_t data, uint8_t pic_num) {

	if (pic_num > 1)
		return;

	uint8_t	reg = (pic_num==1) ? I86_PIC2_REG_DATA : I86_PIC1_REG_DATA;
	outportb (reg, data);
}

static inline uint8_t i86_pic_read_data (uint8_t pic_num) {

	if (pic_num > 1)
		return 0;

	uint8_t	reg = (pic_num==1) ? I86_PIC2_REG_DATA : I86_PIC1_REG_DATA;
	return inportb (reg);
}

// Initialize pic
void i86_pic_initialize (uint8_t base0, uint8_t base1) {

	uint8_t		icw	= 0;

	// Begin initialization of PIC

	icw = (icw & ~I86_PIC_ICW1_MASK_INIT) | I86_PIC_ICW1_INIT_YES;
	icw = (icw & ~I86_PIC_ICW1_MASK_IC4) | I86_PIC_ICW1_IC4_EXPECT;

	i86_pic_send_command (icw, 0);
	i86_pic_send_command (icw, 1);

	//! Send initialization control word 2. This is the base addresses of the irq's

	i86_pic_send_data (base0, 0);
	i86_pic_send_data (base1, 1);

	// Send initialization control word 3. This is the connection between master and slave.
	// ICW3 for master PIC is the IR that connects to secondary pic in binary format
	// ICW3 for secondary PIC is the IR that connects to master pic in decimal format

	i86_pic_send_data (0x04, 0);
	i86_pic_send_data (0x02, 1);

	// Send Initialization control word 4. Enables i86 mode

	icw = (icw & ~I86_PIC_ICW4_MASK_UPM) | I86_PIC_ICW4_UPM_86MODE;

	i86_pic_send_data (icw, 0);
	i86_pic_send_data (icw, 1);
}
