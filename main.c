/*
   main.c
   Stroop: A game based on the stroop effect
   
   Created by Ryan Burton on 2008-10-06.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#include <SDL_ttf.h>

#ifdef GP2X
#include <unistd.h>
#endif

#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_Y               (14)
#define GP2X_BUTTON_X               (15)
#define GP2X_BUTTON_L               (10)
#define GP2X_BUTTON_R               (11)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

int fullscreen = 1;

int pollinput(SDL_Event *event) {
	int done = 0;
	while(SDL_PollEvent(event)) {
		switch(event->type) {
			case SDL_JOYBUTTONUP:
                switch(event->jbutton.button) {
                    case GP2X_BUTTON_L:
                        break;
                    case GP2X_BUTTON_R:
                        break;
					case GP2X_BUTTON_Y:
						break;
                    default: {
                        done = 1;
                        break;
					}
                }
                break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_KEYDOWN:
				switch(event->key.keysym.sym) {
					case SDLK_f: {
						fullscreen = !fullscreen;
						break;
					}
					case SDLK_ESCAPE: {
						done = 1;
						break;
					}
					default:
						break;
				}
                break;
            case SDL_QUIT:
                done = 1;
                break;
            default:
                break;
		}
	}
	return done;
}

void draw_next_word(SDL_Surface *screen, TTF_Font *font, int trial) {
	char* colour_names[] = {"Red", "Blue", "Green", "Yellow"};
	const SDL_Color red = { 0xF0, 0x23, 0x11 };
	const SDL_Color blue = { 0x1E, 0x90, 0xFF };
	const SDL_Color green = { 0x00, 0xFA, 0x9A };
	const SDL_Color yellow = { 0xFF, 0xFF, 0x00 };
	const SDL_Color colour_values[] = {red, blue, green, yellow};
	const int NUMBER_OF_COLOURS = 4;
	static int last_colour_index;
	Uint8 colour_index;
	
	while ((colour_index = rand() % NUMBER_OF_COLOURS) == last_colour_index);
	
	SDL_Color c;
	if(trial) {
		c = colour_values[colour_index];
	} else {
		c = colour_values[rand() % NUMBER_OF_COLOURS];
	}
	SDL_Surface *text = TTF_RenderText_Blended(font, colour_names[colour_index], c);
	
	SDL_Rect text_pos = {(screen->w - text->w)/2, (screen->h - text->h)/2, text->w, text->h};
	SDL_FillRect(screen, NULL, 0);
	SDL_BlitSurface(text, NULL, screen, &text_pos);
	SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
	last_colour_index = colour_index;
	SDL_FreeSurface(text);
}

TTF_Font* get_font(TTF_Font *font, int screen_height) {
	const int SIZEFACTOR = 4;
	if(font) {
		TTF_CloseFont(font);
	}
	
	font = TTF_OpenFont("DroidSans.ttf", screen_height / SIZEFACTOR);
	if(!font) {
		fprintf(stderr, "%s. Is the font there?\n", TTF_GetError());
		exit(2);
	}
	return font;
}

int main(int argc, char* argv[]) {
	Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
	SDL_Surface *screen;
	Uint8 video_bpp = 16;
	Uint32 videoflags = SDL_SWSURFACE | SDL_FULLSCREEN;
	SDL_Rect **video_modes;
	static TTF_Font *font;
	int done = 0;
	SDL_Event event;
	const int WORD_DELAY = 45; // frames
	int countdown_timer = WORD_DELAY;
	int trial_attempts = 10;
	
	if(SDL_Init(flags) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	
	video_modes = SDL_ListModes(NULL, videoflags);
	if(video_modes == (SDL_Rect**)0) {
		fprintf(stderr, "No video modes available.\n");
	}
	if(video_modes == (SDL_Rect**)-1) {
		printf("All resolutions available.\n");
	}
	int i;
	for(i = 0; video_modes[i]; ++i) {
		screen = SDL_SetVideoMode(video_modes[i]->w, video_modes[i]->h, video_bpp, videoflags);
		if(screen != NULL) break;
	}
	if(screen == NULL) {
		fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}
	if(TTF_Init() < 0) {
		fprintf(stderr, "Couldn't initialize text renderer: %s\n", TTF_GetError());
		exit(2);
	}
	if(SDL_JoystickOpen(0) < 0) {
		fprintf(stderr, "Couldn't initialize joystick.\n");
	}
	
	font = get_font(font, video_modes[i]->h);
	
	SDL_ShowCursor(0);
	SDL_WM_SetCaption("Stroop", NULL);
	
	srand(time(NULL));
	
	while(!done) {
		static long timestamp ;
		static long last_timestamp;
		timestamp = SDL_GetTicks();
		if((timestamp-last_timestamp) < 33)
			continue;
		if(countdown_timer <= 0) {
			int trial = (trial_attempts > 0) ? 1 : 0;
			if(trial) trial_attempts--;
			draw_next_word(screen, font, trial);
			countdown_timer = WORD_DELAY;
		} else {
			countdown_timer--;
		}
		done = pollinput(&event);
		if(fullscreen && !(videoflags & SDL_FULLSCREEN)) {
			videoflags = videoflags ^ SDL_FULLSCREEN;
			screen = SDL_SetVideoMode(video_modes[i]->w, video_modes[i]->h, video_bpp, videoflags);
			font = get_font(font, video_modes[i]->h);
		}
		if(!fullscreen && (videoflags & SDL_FULLSCREEN)) {
			videoflags = videoflags ^ SDL_FULLSCREEN;
			screen = SDL_SetVideoMode(640, 480, video_bpp, videoflags);
			font = get_font(font, 480);
		}
		last_timestamp = timestamp;
	}
	
	TTF_CloseFont(font);
	SDL_JoystickClose(0);
	TTF_Quit();
	SDL_Quit();
	#ifdef GP2X
	chdir("/usr/gp2x");
	execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
	#endif
	
	return 0;
}

int WinMain(int argc, char* argv[]) {
	return main(argc, argv);
}
