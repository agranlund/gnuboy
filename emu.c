


#include "defs.h"
#include "regs.h"
#include "hw.h"
#include "cpu.h"
#include "mem.h"
#include "lcd.h"
#include "rc.h"
#include "fb.h"
#include "rtc.h"
#include "sys.h"
#include "sound.h"
#include "cpu.h"


static int framelen = 16743;
static int framecount;
static int paused;

rcvar_t emu_exports[] =
{
	RCV_INT("framelen", &framelen, ""),
	RCV_INT("framecount", &framecount, ""),
	RCV_END
};



void emu_pause(int dopause) {
	paused = dopause;
}

int emu_paused(void) {
	return paused;
}

void emu_init()
{
	
}


/*
 * emu_reset is called to initialize the state of the emulated
 * system. It should set cpu registers, hardware registers, etc. to
 * their appropriate values at powerup time.
 */

void emu_reset()
{
	hw_reset();
	lcd_reset();
	cpu_reset();
	mbc_reset();
	sound_reset();
	mem_mapbootrom();
}





void emu_step()
{
	cpu_emulate(cpu.lcdc);
}



/* This mess needs to be moved to another module; it's just here to
 * make things work in the mean time. */

void *sys_timer();

#if defined(ATARI)
extern void pcm_sync();

void emu_run()
{
	short disp = 1;
	void *timer = sys_timer();
	int delay;

	vid_begin();
	lcd_begin();
	for (;;)
	{
		if (cpu.speed) {
			cpu_emulate(cpu.lcdc * 144);
		} else {
			cpu_emulate(2280);
		}
		while (R_LY > 0 && R_LY < 144) {
			emu_step();
		}

		if (disp&1) {
			vid_end();
			fb.enabled = 0;
		} else {
			fb.enabled = 1;
		}

		disp++;
		if (disp > 1) {
			disp = 0;
		}

		rtc_tick();

		if ((disp&1)) {
			pcm_sync();
		}

		doevents();
		if (paused) {
			disp = fb.enabled = 1;
			return;
		}

		if (disp&1) {
			vid_begin();
		}

		if (!(R_LCDC & 0x80)) {
			cpu_emulate(32832);
		}
		while (R_LY > 0) {
			emu_step();
		}
	}
}

#else

void emu_run()
{
	void *timer = sys_timer();
	int delay;

	vid_begin();
	lcd_begin();
	for (;;)
	{
		cpu_emulate(2280);
		while (R_LY > 0 && R_LY < 144)
			emu_step();
		
		vid_end();
		rtc_tick();
		sound_mix();
		if (!pcm_submit())
		{
			delay = framelen - sys_elapsed(timer);
			sys_sleep(delay);
			sys_elapsed(timer);
		}
		doevents();
		if (paused) return;
		vid_begin();
		if (framecount) { if (!--framecount) die("finished\n"); }
		if (!(R_LCDC & 0x80))
			cpu_emulate(32832);
		
		while (R_LY > 0) /* wait for next frame */
			emu_step();
	}
}

#endif











