#ifndef __dm5408_mod_h__
#define __dm5408_mod_h__

#define IOBASE	0x0300	// Default Base Address
#define IOSPACE	16		// IO Space Range
#define IRQS	2		// Number of IRQ's
#define DMAS	2		// Number of DMA's
#define FREQ	8000000	// Working Freqency

// DM5408 Device Structure with spinlock
typedef struct dm5408_device
{
	ushort		io_base;
	ushort		control_register,
				datain_register;
	atomic_t	inuse_counter;
	spinlock_t	spin_lock;
} dm5408_device_t ;

#endif //__dm5408_mod_h__
