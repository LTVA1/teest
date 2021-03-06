/*

Example klystrack command line replayer. Use as you like.
Usage: player.exe <song>

*/

/* SDL stuff */

#include "SDL.h"

/* klystron stuff */

#include "snd/cyd.h"
#include "snd/music.h"

#include <string.h>

#undef main

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <song>\n", argv[0]);
		return 1;
	}
	
	MusSong song;
	CydEngine cyd;
	MusEngine mus;
	
	memset(&song, 0, sizeof(song));
	
	/* To be sure, let's init enough channels */
	
	cyd_init(&cyd, 44100, MUS_MAX_CHANNELS);
	
	if (!mus_load_song(argv[1], &song, cyd.wavetable_entries))
	{
		fprintf(stderr, "Could not open %s\n", argv[1]);
		cyd_deinit(&cyd);
		return 2;
	}

	SDL_Init(SDL_INIT_AUDIO);
		
	/* Notify the music engine about Cyd */
	
	mus_init_engine(&mus, &cyd);
	
	/* Add Cyd in SDL_Mixer audio output queue */ 
	
	cyd_register(&cyd, 2000);     
	
	/* Start updating the music engine at the rate set in the song */
	
	cyd_set_callback(&cyd, mus_advance_tick, &mus, song.song_rate);
	
	/* Start playing from position 0 */
	
	mus_set_song(&mus, &song, 0);
	
	printf("Playing %s...\n\nPress Ctrl-C to exit.\n\n", song.title);
	
	int done = 0;
	
	while (!done)
	{
		SDL_Event e;
		
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
				case SDL_KEYDOWN:
					done = 1;
					break;
			}
		}
		
		int song_position;
		
		mus_poll_status(&mus, &song_position, NULL, NULL, NULL, NULL, NULL, NULL);
		
		printf("Position: %4d/%d\r", song_position, song.song_length);
		
		SDL_Delay(5);
	}
	
	printf("\nQuit.\n");
	
	cyd_unregister(&cyd); 
	
	cyd_deinit(&cyd);
	
	cyd_unregister(&cyd);
	cyd_deinit(&cyd);
	
	mus_free_song(&song);
	
	SDL_Quit();
	
	return 0;
}
