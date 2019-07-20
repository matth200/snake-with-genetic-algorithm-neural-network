#include <fstream>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <cmath>

//random function
#include <cstdlib>
#include <ctime>

#include <chrono>
#include <thread>
//this_thread::sleep_for();
typedef std::chrono::high_resolution_clock::time_point time_point;

#include "snake.h"

#define FPS 30.0

using namespace std;

int main ( int argc, char** argv )
{

	ofstream log("log.txt");
	srand(time(NULL));
	log << "init" << endl;
	// initialize SDL video
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
        	log << "init sdl :" << SDL_GetError() << endl;
        	return 1;
	}
	//initialize TTF
	if( TTF_Init() < 0)
	{
		log << "problem with ttf_init" << endl;
		return 1;
	}

	// make sure SDL cleans up before exit
	atexit(SDL_Quit);
	atexit(TTF_Quit);

    // create a new window
    SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT+100, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if (!screen)
	{
    	log << "Unable to set video: " << SDL_GetError() << endl;
		return 1;
	}

	SDL_WM_SetCaption("SNAKE",NULL);

	//main loop
	bool continuer = 1;
	SDL_Event event;

	//snake
	Snake snake(60,60);

	double fpsDirect = 0.0;
	time_point timeBefore, timeNow, timeFirst = chrono::high_resolution_clock::now();

	while(continuer)
	{
		timeBefore = chrono::high_resolution_clock::now();
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					continuer = 0;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_DOWN:
							snake.move(3);
							break;
						case SDLK_UP:
							snake.move(2);
							break;
						case SDLK_RIGHT:
							snake.move(1);
							break;
						case SDLK_LEFT:
							snake.move(0);
							break;
					}
					break;
			}
		}
		SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
		
		snake.draw(screen);
		drawSquare(screen,0,SCREEN_HEIGHT,SCREEN_WIDTH,3,SDL_MapRGB(screen->format,255,255,255));

		SDL_Flip(screen);
		//actualisation

		timeNow = chrono::high_resolution_clock::now();
		if(chrono::duration_cast<chrono::milliseconds>(timeNow-timeBefore).count()>1000.0/FPS)
		{
			this_thread::sleep_for(chrono::milliseconds((unsigned int)(chrono::duration_cast<chrono::milliseconds>(timeNow-timeBefore).count()-1000.0/FPS)));
		}

		fpsDirect = 1000.0/chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()-timeBefore).count();
	}
	return 0;
}
