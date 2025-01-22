#include "defs.h"
#include "pcm.h"
#include "rc.h"
#include "sys.h"
#include <string.h>

extern int  pcm_init_null();
extern void pcm_close_null();
extern int  pcm_submit_null();
extern void pcm_sync_null();
extern void pcm_pause_null(int dopause);

extern int  pcm_init_sb();
extern void pcm_close_sb();
extern int  pcm_submit_sb();
extern void pcm_sync_sb();
extern void pcm_pause_sb(int dopause);

extern int  pcm_init_sdl();
extern void pcm_close_sdl();
extern int  pcm_submit_sdl();
extern void pcm_sync_sdl();
extern void pcm_pause_sdl(int dopause);

void (*atari_pcm_close)(void);
int  (*atari_pcm_submit)(void);
void (*atari_pcm_sync)(void);
void (*atari_pcm_pause)(int);


/* -----------------------------------------------------------------------------------
 * 
 * Gnuboy
 * 
 * ---------------------------------------------------------------------------------*/

struct pcm pcm;

rcvar_t pcm_exports[] =
{
	RCV_END
};


void pcm_init()
{
    if (1 && pcm_init_sb()) {
        atari_pcm_close = pcm_close_sb;
        atari_pcm_submit = pcm_submit_sb;
        atari_pcm_sync = pcm_sync_sb;
        atari_pcm_pause = pcm_pause_sb;
    }
    else if (1 && pcm_init_sdl()) {
        atari_pcm_close = pcm_close_sdl;
        atari_pcm_submit = pcm_submit_sdl;
        atari_pcm_sync = pcm_sync_sdl;
        atari_pcm_pause = pcm_pause_sdl;
    }
    else if (1 && pcm_init_null()) {
        atari_pcm_close = pcm_close_null;
        atari_pcm_submit = pcm_submit_null;
        atari_pcm_sync = pcm_sync_null;
        atari_pcm_pause = pcm_pause_null;
    }
}

void pcm_close() {
    atari_pcm_close();
	memset(&pcm, 0, sizeof pcm);
}

int pcm_submit() {
    return atari_pcm_submit();
}

void pcm_sync() {
    atari_pcm_sync();
}

void pcm_pause(int dopause) {
    atari_pcm_pause(dopause);
}
