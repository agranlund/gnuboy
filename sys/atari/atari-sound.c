#include "defs.h"
#include "pcm.h"
#include "rc.h"
#include "sys.h"
#include <string.h>

extern int  pcm_init_null();
extern void pcm_close_null();
extern int  pcm_submit_null();
extern void pcm_sync_null();
extern void pcm_sync_null_off();
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


static int sound = 1;
static int sound_null = 1;
static int sound_sdl = 1;
static int sound_sb = 1;

rcvar_t pcm_exports[] =
{
    RCV_BOOL("sound", &sound, "sound enabled"),
#if SOUND_ENABLE_SOUNDBLASTER
    RCV_BOOL("sbsound",   &sound_sb,  "enable soundblaster driver"),
#endif
#if SOUND_ENABLE_SDL
    RCV_BOOL("sdlsound",  &sound_sdl, "enable sdl sound driver"),
#endif    
    RCV_BOOL("nullsound", &sound_null, "enable null sound driver"),
	RCV_END
};

void pcm_init()
{
    if (SOUND_ENABLE_SOUNDBLASTER && sound && sound_sb && pcm_init_sb()) {
        atari_pcm_close = pcm_close_sb;
        atari_pcm_submit = pcm_submit_sb;
        atari_pcm_sync = pcm_sync_sb;
        atari_pcm_pause = pcm_pause_sb;
    }
    else if (SOUND_ENABLE_SDL && sound && sound_sdl && pcm_init_sdl()) {
        atari_pcm_close = pcm_close_sdl;
        atari_pcm_submit = pcm_submit_sdl;
        atari_pcm_sync = pcm_sync_sdl;
        atari_pcm_pause = pcm_pause_sdl;
    }
    else {
        pcm_init_null();
        atari_pcm_close = pcm_close_null;
        atari_pcm_submit = pcm_submit_null;
        atari_pcm_sync = sound_null ? pcm_sync_null : pcm_sync_null_off;
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
