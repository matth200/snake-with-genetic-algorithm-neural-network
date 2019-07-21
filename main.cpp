#include <fstream>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <cmath>

//random function
#include <cstdlib>
#include <ctime>

#include <vector>

#include <chrono>
#include <thread>
//this_thread::sleep_for();
typedef std::chrono::high_resolution_clock::time_point time_point;

#include "snake.h"

#include "m_learning.h"

#define FPS 30.0

//parametre GENETIC_ALGORITHM
#define NBR_SELECTION 20
#define FRQ_MUTATION 0.05

using namespace std;

void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m);

//variable pour effectuer la selection
struct VarSelection
{
	MachineLearning m;
	int score;
};

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

	TTF_Font *police = TTF_OpenFont("pixel_font.ttf",23);

	//variable pour la selection
	vector<VarSelection> snakeSelection;

	VarSelection tmpSelection;
	tmpSelection.m.open(8);

	// make sure SDL cleans up before exit
	atexit(SDL_Quit);
	atexit(TTF_Quit);

    // create a new window
    SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH+OUTSCREEN_W, SCREEN_HEIGHT+OUTSCREEN_H, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if (!screen)
	{
    	log << "Unable to set video: " << SDL_GetError() << endl;
		return 1;
	}

	SDL_WM_SetCaption("SNAKE",NULL);

	//main loop
	bool continuer = 1;
	SDL_Event event;

	//IA snake
	bool autonome = 0;

	MachineLearning playerIA(8);
	playerIA.addColumn(6);
	playerIA.addColumn(6);
	playerIA.addColumn(4);

	//random
	playerIA.setWeightRandom(70,70);

	//snake
	Snake snake(60,60);

	//gestion du temps et des fps
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
					if(!autonome){
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
							case SDLK_RETURN:
								autonome = 1;
								break;
						}
					}else{
						switch(event.key.keysym.sym)
						{
							case SDLK_RETURN:
								autonome = 0;
								break;
						}
					}
					break;
			}
		}
		//vitesse du serpent
		snake.set_speed(5);
		
		//mise à jour des informations dans le réseaux de neurone
		char *data = snake.getRangeWall();
		playerIA.setInput(data);

		playerIA.calcul();
		
		//mouvement IA
		if(autonome)
			snake.move(playerIA.getPrediction());

		//init screen
		SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
		
		//on affiche les infos du réseaux de neurone
		drawNeuralNetwork(screen,playerIA);

		//dessin des mouvements du serpent
		snake.draw(screen);	

		if(snake.gameover())
		{
			playerIA.setWeightRandom(500,500);
		}

		//barre qui sépare le score du jeu
		drawSquare(screen,0,SCREEN_HEIGHT,SCREEN_WIDTH,3,SDL_MapRGB(screen->format,255,255,255));
		drawSquare(screen,SCREEN_WIDTH,0,3,SCREEN_HEIGHT+SCREEN_HEIGHT,SDL_MapRGB(screen->format,255,255,255));

		//affichage du mode autonome
		if(autonome)
			drawSquare(screen,25,SCREEN_HEIGHT+60,30,30,SDL_MapRGB(screen->format,25,255,25));

		//actualisation
		SDL_Flip(screen);

		timeNow = chrono::high_resolution_clock::now();
		if(chrono::duration_cast<chrono::milliseconds>(timeNow-timeBefore).count()>1000.0/FPS)
		{
			this_thread::sleep_for(chrono::milliseconds((unsigned int)(chrono::duration_cast<chrono::milliseconds>(timeNow-timeBefore).count()-1000.0/FPS)));
		}

		fpsDirect = 1000.0/chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()-timeBefore).count();
	}
	TTF_CloseFont(police);
	return 0;
}

void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m)
{
	for(int j(0);j<m.getNumberColumn();j++)
	{
		for(int i(0);i<m.getNetwork(j)->get_number_neuron();i++)
		{
			double value = m.getNetwork(j)->get_neuron(i)->get_value();
			drawSquare(screen,SCREEN_WIDTH+30+j*80,50+i*60,40,40,SDL_MapRGB(screen->format,value*255.0,value*255.0,value*255.0));
		}
	}
}