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

#define FPS 30

//parametre GENETIC_ALGORITHM
#define NBR_POPULATION 10
#define FRQ_MUTATION 0.02
#define MIXADN_CURSOR 0.7
#define NBR_SELECTION 70

#define MOVES_LEFT 200

#define RANDOM_VALUE 100

using namespace std;

//variable pour effectuer la selection
struct VarSelection
{
	MachineLearning m;
	int score;
	bool best;
};


void drawNeuralNetwork(SDL_Surface *screen, MachineLearning &m);
void getAdn(MachineLearning &m, vector<double> &adn);
void setAdn(MachineLearning &m, vector<double> &adn);
void makeBabys(MachineLearning &m1, MachineLearning &m2);
VarSelection selectionRandomly(vector<VarSelection> &players);
double distance(int x1, int y1, int x2, int y2);

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

	TTF_Font *police = TTF_OpenFont("pixel_font.ttf",20);

	//variable pour la selection
	vector<VarSelection> snakeSelection, playerSelection;

	VarSelection tmpSelection;
	bool selectionReady = 0;

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
	int speed = 3;
	bool buttonSpeed = 0;

	//IA snake
	bool autonome = 1;

	MachineLearning playerIA(24);
	playerIA.addColumn(18);
	playerIA.addColumn(4);

	//random
	playerIA.setWeightRandom(RANDOM_VALUE,RANDOM_VALUE);

	//snake
	Snake snake(60,60);
	snake.setMove(MOVES_LEFT);

	//genetic algorithm
	int generation = 1, oldGeneration = 0;

	SDL_Surface *texteGeneration = NULL;
	SDL_Rect posTexte;
	posTexte.x = SCREEN_WIDTH+10;
	posTexte.y = SCREEN_HEIGHT+70;

	VarSelection best_IA;
	best_IA.m = playerIA;
	best_IA.score = 0;
	best_IA.best = 1;

	//gestion du temps et des fps
	double fpsDirect = 0.0;
	time_point timeBefore, timeNow, timeFirst = chrono::high_resolution_clock::now();

	while(continuer)
	{
		//on releve le temps pour controler les fps
		timeBefore = chrono::high_resolution_clock::now();
		//on fait une boucle de tous les événement qui se sont passé pendant notre absence
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				//pour quitter le logiciel
				case SDL_QUIT:
					continuer = 0;
					break;
				//control par les touches
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_s:
							if(!autonome){
								snake.move(3);
							}
							break;
						case SDLK_z:
							if(!autonome){
								snake.move(2);
							}
							break;
						case SDLK_d:
							if(!autonome){
								snake.move(1);
							}
							break;
						case SDLK_q:
							if(!autonome){
								snake.move(0);
							}
							break;

						case SDLK_RETURN:
							if(autonome)
								autonome = 0;
							else
								autonome = 1;
							break;
						case SDLK_ESCAPE:
							continuer = 0;
							break;
						case SDLK_RIGHT:
							if(!buttonSpeed)
							{
								speed++;
								buttonSpeed = 1;
							}
							break;
						case SDLK_LEFT:
							if(!buttonSpeed)
							{
								if(speed>1)
									speed--;
								buttonSpeed = 1;
							}
							break;
					}
					break;

				case SDL_KEYUP:
					switch(event.key.keysym.sym)
					{
						case SDLK_RIGHT:
							buttonSpeed = 0;
							break;
						case SDLK_LEFT:
							buttonSpeed = 0;
							break;
					}
					break;
			}
		}

		//init screen
		SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));

		//vitesse du serpent
		snake.set_speed(speed);
		
		//on entre les distances de la tete du serpent par rapport au mur dans 8 directions dans le réseaux de neurone
		char *data = snake.getRangeWall();
		playerIA.setInput(data,8,0);
		data = snake.getRangeQueue();
		playerIA.setInput(data,8,8);
		data = snake.getRangeFood();
		playerIA.setInput(data,8,16);

		//mise à jour des informations dans le réseaux de neurone
		playerIA.calcul();
		
		//mouvement IA
		if(autonome)
			snake.move(playerIA.getPrediction());
		
		//on affiche les infos du réseaux de neurone
		drawNeuralNetwork(screen,playerIA);

		//dessin des mouvements du serpent
		snake.draw(screen);	

		//affichage du mode autonome
		if(autonome)
			drawSquare(screen,25,SCREEN_HEIGHT+60,30,30,SDL_MapRGB(screen->format,25,255,25));

		//barre qui sépare le score du jeu
		drawSquare(screen,0,SCREEN_HEIGHT,SCREEN_WIDTH,3,SDL_MapRGB(screen->format,255,255,255));
		drawSquare(screen,SCREEN_WIDTH,0,3,SCREEN_HEIGHT+OUTSCREEN_H,SDL_MapRGB(screen->format,255,255,255));

		//on affiche les générations et le score de la meilleure IA
		if(oldGeneration!=generation)
		{
			//on efface l'ancien texte
			if(texteGeneration!=NULL)
				SDL_FreeSurface(texteGeneration);
			texteGeneration = TTF_RenderText_Solid(police,(string("Generation ")+(to_string(generation)+" best_IA score ")+to_string(best_IA.score)).c_str(),SDL_Color({255,255,255}));
		}
		SDL_BlitSurface(texteGeneration,NULL,screen,&posTexte);

		//quand le serpent meurt
		if(snake.gameover())
		{
			//on fait jouer les IA en arrière plan
			while(NBR_POPULATION>playerSelection.size()&&autonome)
			{
				tmpSelection.score = int(snake.get_step()+pow(snake.get_score()*10,2)*100);
				snake.init_after();
				tmpSelection.m = playerIA;
				tmpSelection.best = 0;
				playerSelection.push_back(tmpSelection);

				//on init le nouveau joueur
				snake.setMove(MOVES_LEFT);

				if(NBR_POPULATION>playerSelection.size())
				{
					//old player 
					if(snakeSelection.size()>0){
						playerIA = snakeSelection[0].m;
						if(snakeSelection[0].best)
							log << "bestPlayer play" << endl;
						snakeSelection.erase(snakeSelection.begin());

						log << "Babys Player" << endl;
					}
					//new player with random gene 
					else{
							playerIA.setWeightRandom(RANDOM_VALUE,RANDOM_VALUE);
							log << "Random Player" << endl;
					}

					//tant qu'il n'a pas perdu il joue
					while(!snake.gameover())
					{
						//on entre les distances de la tete du serpent par rapport au mur dans 8 directions dans le réseaux de neurone
						char *data = snake.getRangeWall();
						playerIA.setInput(data,8,0);
						data = snake.getRangeQueue();
						playerIA.setInput(data,8,8);
						data = snake.getRangeFood();
						playerIA.setInput(data,8,16);

						//mise à jour des informations dans le réseaux de neurone
						playerIA.calcul();

						//ensuite l'IA choisit la direction
						snake.move(playerIA.getPrediction());

						//on bouge le serpent de un bloc
						snake.draw(screen,1);
					}
				}
			}
			//on manipule la population pour obtenir de meilleur résultat
			if(autonome&&NBR_POPULATION<=playerSelection.size()){
				selectionReady = 1;

				//selection
				vector<VarSelection> comparaisonListe(NBR_POPULATION);
				for(int i(0);i<NBR_POPULATION;i++)
				{
					comparaisonListe[i].score = 0;
					comparaisonListe[i].best = 0;
				}
				snakeSelection = playerSelection;
				playerSelection.clear();

				//on mets dans l'ordre
				bool done;
				for(int j(0);j<snakeSelection.size();j++)
				{
					done = 0;
					for(int i(0);i<comparaisonListe.size()&&!done;i++)
					{
						if(comparaisonListe[i].score<snakeSelection[j].score)
						{
							VarSelection transfer, newSelection = snakeSelection[j];
							for(i;i<comparaisonListe.size();i++)
							{
								transfer = comparaisonListe[i];
								comparaisonListe[i] = newSelection;
								newSelection = transfer;
							}
							done = 1;
						}
					}
				}

				//on récupére le gagnant si il est meilleur que ceux des génération d'avant
				if(comparaisonListe[0].score>best_IA.score)
				{
					comparaisonListe[0].best = 1;
					best_IA = comparaisonListe[0];
				}

				//affichage de la liste
				log << "generation " << generation << endl;
				for(int i(0);i<comparaisonListe.size();i++)
				{
					log << i << ": " << comparaisonListe[i].score << " | ";
				}
				log << endl;


				snakeSelection.clear();

				for(int i(0);i<NBR_SELECTION;i++)
					snakeSelection.push_back(comparaisonListe[i]);

				//selection and create babys finally
				vector<VarSelection> copy = snakeSelection;

				while(snakeSelection.size()<NBR_SELECTION*2.0)
				{
					VarSelection parent1 = selectionRandomly(copy), parent2 = selectionRandomly(copy);
					
					//init babys
					parent1.best=0;
					parent2.best=0;
					parent1.score=0;
					parent2.score=0;

					//parents become babyssss
					makeBabys(parent1.m,parent2.m);

					//ajout dans la liste
					snakeSelection.push_back(parent1);
					snakeSelection.push_back(parent2);
				}
				log << "create babys okay" << endl;

				//mutation
				for(int i(0);i<snakeSelection.size();i++)
				{
					//get adn
					vector<double> adn;
					getAdn(snakeSelection[i].m,adn);

					//we gonna mutate this babyyyy
					const int randomNumber = RANDOM_VALUE;
					for(int j(0);j<adn.size();j++){
						if(rand()%1000+1<=FRQ_MUTATION*1000.0)
						{
							adn[j] = double(rand()%(randomNumber*1000)/1000.0-double(randomNumber)/2.0);
							log << "M";
						}
					}
					log << " & " << endl;
					//set adn
					setAdn(snakeSelection[i].m,adn);
				}

				log << "mutation okay " << endl;

				//on met le premier sans mutation
				snakeSelection.push_back(best_IA);

				log << "number of neural network in the snakeSelection  " << snakeSelection.size() << endl;

				//on indique qu'on passe à la génération d'au dessus
				generation++;
				selectionReady = 0;
			}

			//on initialise le snake
			snake.init();
			snake.init_after();
			
			snake.setMove(MOVES_LEFT);

			//on sélectionne le premier de la liste pour le faire jouer à l'écran
			playerIA = snakeSelection[0].m;
			if(snakeSelection[0].best)
				log << "bestPlayer play" << endl;
			snakeSelection.erase(snakeSelection.begin());

			log << "Babys Player to the screen" << endl;
		}

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
			drawSquare(screen,SCREEN_WIDTH+30+j*60,50+i*30,20,20,SDL_MapRGB(screen->format,value*255.0,value*255.0,value*255.0));
		}
	}
}

void makeBabys(MachineLearning &m1, MachineLearning &m2)
{
	//get the adn 
	vector<double> adn1, adn2, adnT1, adnT2;
	//pour qu'il prenne les meme réseaux de neurone
	getAdn(m1,adn1);
	getAdn(m2,adn2);
	ofstream log("logBabys.txt");

	//mix the adn <<<----
	log << "adn |";
	for(int i(0);i<adn1.size();i++)
	{
		if(rand()%1000<MIXADN_CURSOR*1000.0)
		{
			log << "O";
			adnT1.push_back(adn1[i]);
			adnT2.push_back(adn2[i]);
		}else{
			log << "M";
			adnT1.push_back(adn2[i]);
			adnT2.push_back(adn1[i]);
		}
	}

	//set the adn
	setAdn(m1,adnT1);
	setAdn(m2,adnT2);
}

void getAdn(MachineLearning &m, vector<double> &adn)
{
	adn.clear();
	for(int l(0);l<m.getNumberColumn()-1;l++)
	{
		for(int j(0);j<m.getNetwork(l+1)->get_number_neuron();j++)
		{
			for(int i(0);i<m.getNetwork(l+1)->get_neuron(j)->numberConnection();i++)
			{
				adn.push_back(m.getNetwork(l+1)->get_neuron(j)->get_weight(i));
			}
			adn.push_back(m.getNetwork(l+1)->get_neuron(j)->get_bias());
		}
	}
}

void setAdn(MachineLearning &m, vector<double> &adn)
{
	int index = 0;
	for(int l(0);l<m.getNumberColumn()-1;l++)
	{
		for(int j(0);j<m.getNetwork(l+1)->get_number_neuron();j++)
		{
			for(int i(0);i<m.getNetwork(l+1)->get_neuron(j)->numberConnection();i++)
			{
				//m.getNetwork(l+1)->get_neuron(j)->set_weight(i,adn[index]);
				index++;
			}
			//m.getNetwork(l+1)->get_neuron(j)->set_bias(adn[index]);
			index++;
		}
	}

	if(adn.size()==index)
	{
		index = 0;
		for(int l(0);l<m.getNumberColumn()-1;l++)
		{
			for(int j(0);j<m.getNetwork(l+1)->get_number_neuron();j++)
			{
				for(int i(0);i<m.getNetwork(l+1)->get_neuron(j)->numberConnection();i++)
				{
					m.getNetwork(l+1)->get_neuron(j)->set_weight(i,adn[index]);
					index++;
				}
				m.getNetwork(l+1)->get_neuron(j)->set_bias(adn[index]);
				index++;
			}
		}
	}
}

VarSelection selectionRandomly(vector<VarSelection> &players)
{
	int sum = 0;
	for(int i(0);i<players.size();i++)
	{
		sum+=players[i].score;
	}

	int arrow = rand()%sum;
	int valeurCum = 0;
	for(int i(0);i<players.size();i++)
	{
		if(valeurCum<=arrow&&players[i].score+valeurCum>arrow)
		{
			VarSelection copy = players[i];
			copy.score = 0;
			players.erase(players.begin()+i);
			return copy;
		}
		valeurCum+=players[i].score;
	}
	VarSelection copy;
	return copy;
}

double distance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}
