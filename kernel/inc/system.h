#ifndef _SYSTEM_H
#define _SYSTEM_H

typedef		unsigned char		bool;
typedef 	signed char		int8_t;
typedef 	unsigned char		uint8_t;
typedef 	short			int16_t;
typedef 	unsigned short		uint16_t;
typedef 	int			int32_t;
typedef 	unsigned		uint32_t;
typedef 	long long		int64_t;
typedef 	unsigned long long 	uint64_t;

#define true 0x1
#define false 0x0

#define VIDMEM 0xb8000

#define DISPLAY_HEIGHT 0x0f
#define DISPLAY_WIDTH  0x50

#define DESCRIPTOR_NUMBER_LIMIT 0x2000
#define INTERRUPT_NUMBER_LIMIT 0x100

// Command Word 2 bit masks. Use when sending commands
#define		I86_PIC_OCW2_MASK_L1		1		//00000001	//Level 1 interrupt level
#define		I86_PIC_OCW2_MASK_L2		2		//00000010	//Level 2 interrupt level
#define		I86_PIC_OCW2_MASK_L3		4		//00000100	//Level 3 interrupt level
#define		I86_PIC_OCW2_MASK_EOI		0x20		//00100000	//End of Interrupt command
#define		I86_PIC_OCW2_MASK_SL		0x40		//01000000	//Select command
#define		I86_PIC_OCW2_MASK_ROTATE	0x80		//10000000	//Rotation command

// Command Word 3 bit masks. Use when sending commands
#define		I86_PIC_OCW3_MASK_RIS		1		//00000001
#define		I86_PIC_OCW3_MASK_RIR		2		//00000010
#define		I86_PIC_OCW3_MASK_MODE		4		//00000100
#define		I86_PIC_OCW3_MASK_SMM		0x20		//00100000
#define		I86_PIC_OCW3_MASK_ESMM		0x40		//01000000
#define		I86_PIC_OCW3_MASK_D7		0x80		//10000000

// Initialization Control Word 1 bit masks
#define I86_PIC_ICW1_MASK_IC4			0x1	//00000001	// Expect ICW 4 bit
#define I86_PIC_ICW1_MASK_SNGL			0x2	//00000010	// Single or Cascaded
#define I86_PIC_ICW1_MASK_ADI			0x4	//00000100	// Call Address Interval
#define I86_PIC_ICW1_MASK_LTIM			0x8	//00001000	// Operation Mode
#define I86_PIC_ICW1_MASK_INIT			0x10	//00010000	// Initialization Command

// Command Control bits for Initialization Control Word 1
#define I86_PIC_ICW1_IC4_EXPECT			1	//1		//Use when setting I86_PIC_ICW1_MASK_IC4
#define I86_PIC_ICW1_IC4_NO			0	//0
#define I86_PIC_ICW1_SNGL_YES			2	//10		//Use when setting I86_PIC_ICW1_MASK_SNGL
#define I86_PIC_ICW1_SNGL_NO			0	//00
#define I86_PIC_ICW1_ADI_CALLINTERVAL4		4	//100		//Use when setting I86_PIC_ICW1_MASK_ADI
#define I86_PIC_ICW1_ADI_CALLINTERVAL8		0	//000
#define I86_PIC_ICW1_LTIM_LEVELTRIGGERED	8	//1000		//Use when setting I86_PIC_ICW1_MASK_LTIM
#define I86_PIC_ICW1_LTIM_EDGETRIGGERED		0	//0000
#define I86_PIC_ICW1_INIT_YES			0x10	//10000		//Use when setting I86_PIC_ICW1_MASK_INIT
#define I86_PIC_ICW1_INIT_NO			0	//00000

// Initialization Control Word 4 bit masks
#define I86_PIC_ICW4_MASK_UPM		0x1	//00000001	// Mode
#define I86_PIC_ICW4_MASK_AEOI		0x2	//00000010	// Automatic EOI
#define I86_PIC_ICW4_MASK_MS		0x4	//00000100	// Selects buffer type
#define I86_PIC_ICW4_MASK_BUF		0x8	//00001000	// Buffered mode
#define I86_PIC_ICW4_MASK_SFNM		0x10	//00010000	// Special fully-nested mode

// Initialization Control Word 4 control bits
#define I86_PIC_ICW4_UPM_86MODE		1	//1		//Use when setting I86_PIC_ICW4_MASK_UPM
#define I86_PIC_ICW4_UPM_MCSMODE	0	//0
#define I86_PIC_ICW4_AEOI_AUTOEOI	2	//10		//Use when setting I86_PIC_ICW4_MASK_AEOI
#define I86_PIC_ICW4_AEOI_NOAUTOEOI	0	//00
#define I86_PIC_ICW4_MS_BUFFERMASTER	4	//100		//Use when setting I86_PIC_ICW4_MASK_MS
#define I86_PIC_ICW4_MS_BUFFERSLAVE	0	//000
#define I86_PIC_ICW4_BUF_MODEYES	8	//1000		//Use when setting I86_PIC_ICW4_MASK_BUF
#define I86_PIC_ICW4_BUF_MODENO		0	//0000
#define I86_PIC_ICW4_SFNM_NESTEDMODE	0x10	//10000		//Use when setting I86_PIC_ICW4_MASK_SFNM
#define I86_PIC_ICW4_SFNM_NOTNESTED	0	//00000

// PIC 1 register port addresses
#define I86_PIC1_REG_COMMAND			0x20			// command register
#define I86_PIC1_REG_STATUS			0x20			// status register
#define I86_PIC1_REG_DATA			0x21			// data register
#define I86_PIC1_REG_IMR			0x21			// interrupt mask register (imr)

// PIC 2 register port addresses
#define I86_PIC2_REG_COMMAND			0xA0			//  see above register names
#define I86_PIC2_REG_STATUS			0xA0
#define I86_PIC2_REG_DATA			0xA1
#define I86_PIC2_REG_IMR			0xA1

uint8_t inportb (uint16_t _port);
void outportb (uint16_t _port, uint8_t _data);
uint16_t inportw (uint16_t _port);
void outportw (uint16_t _port, uint16_t _data);

bool string_compare (char * string_one, char * string_two, uint8_t length);

extern uint8_t gdt_install();
extern uint8_t isrs_install();
extern uint8_t irq_install();
extern void gdt_flush ();

extern uint8_t filesystem_initialize ();
extern uint8_t vmem_initialize ();

void play_sound (uint16_t frequency);
void timer_animate_frame ();

#endif
