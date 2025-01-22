#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include "rc.h"
#include "pcm.h"
#include "sys.h"
extern void* sys_timer();
static void *timer;

static volatile int audio_done;
static int paused;

static void audio_callback(void *blah, byte *stream, int len)
{
	memcpy(stream, pcm.buf, len);
	audio_done = 1;
}

int pcm_init_sdl() {
	int i;
	SDL_AudioSpec as = {0}, ob;
	if (!SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        return 0;
    }

    /*
     * todo: adjust freq/format based on machine
     */

	as.freq = 8194;
	as.channels = 1;
	as.format = AUDIO_U8;
    
	as.samples = as.freq / 30;
	for (i = 1; i < as.samples; i<<=1);
	as.samples = i;
	as.callback = audio_callback;
	as.userdata = 0;
	if (SDL_OpenAudio(&as, &ob) == -1) {
		return 0;
	}

	pcm.hz = ob.freq;
	pcm.stereo = ob.channels - 1;
	pcm.len = ob.size;
	pcm.buf = malloc(pcm.len);
	pcm.pos = 0;
	memset(pcm.buf, 0, pcm.len);

    timer = sys_timer();

	SDL_PauseAudio(0);
    return 1;
}

int pcm_submit_sdl() {
	if (!pcm.buf || paused) {
		pcm.pos = 0;
		return 0;
	}
	if (pcm.pos < pcm.len) return 1;
	while (!audio_done) {
		SDL_Delay(4);
    }
	audio_done = 0;
	pcm.pos = 0;
	return 1;
}

void pcm_sync_sdl() {
    if (!pcm_submit_sdl()) {
        int framelen = (16743 << 1);
        int delay = framelen - sys_elapsed(timer);
        if (delay > 0) {
            sys_sleep(delay);
        }
        sys_elapsed(timer);
    }
    pcm.pos = 0;
}

void pcm_close_sdl() {
	SDL_CloseAudio();
}

void pcm_pause_sdl(int dopause) {
	paused = dopause;
	SDL_PauseAudio(paused);
}
