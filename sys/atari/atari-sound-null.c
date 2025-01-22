#include "defs.h"
#include "pcm.h"
#include "rc.h"
#include "sys.h"
#include <string.h>
extern void* sys_timer();
static void *timer;

static byte buf[4096];

int pcm_init_null()
{
	pcm.hz  = 11025;
	pcm.buf = buf;
	pcm.len = sizeof buf;
	pcm.pos = 0;
    pcm.stereo = 0;
    timer = sys_timer();
}

void pcm_close_null() {
}

int pcm_submit_null() {
    pcm.pos = 0;
	return 0;
}

void pcm_sync_null_off() {
    pcm.pos = 0;
}

void pcm_sync_null() {
    if (!pcm_submit_null()) {
        int framelen = (16743 << 1);
        int delay = framelen - sys_elapsed(timer);
        if (delay > 0) {
            sys_sleep(delay);
        }
        sys_elapsed(timer);
    }
    pcm.pos = 0;
}

void pcm_pause_null(int dopause) {
}

