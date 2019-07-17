#include <fstream>
#include <SDL/SDL.h>

#define FPS 30
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 700

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawLine(SDL_Surface *screen, int x, int y, int w, int h, Uint32 color);

int main ( int argc, char** argv )
{
	ofstream log("log.txt");
	log << "init" << endl;
	// initialize SDL video
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
        	cout << "init sdl :" << SDL_GetError() << endl;
        	return 1;
	}
	// make sure SDL cleans up before exit
	atexit(SDL_Quit);
	
    	// create a new window
    	SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    	if (!screen)
    	{
        	log << "Unable to set video: " << SDL_GetError() << endl;
		return 1;
	}
	return 0;
}


void setPixel(SDL_Surface *screen, int x, int y, Uint32 color)
{
	*((Uint32*)(screen->pixels)+x+y*screen->w) = color;
}

void drawLine(SDL_Surface *screen, int x, int y, int w, int h, Uint32 color)
{

}
