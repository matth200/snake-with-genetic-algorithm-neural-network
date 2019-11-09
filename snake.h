#ifndef INCLUDE_SNAKE
#define INCLUDE_SNAKE

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

//pour stringstream et mettre des flottants fixe
#include <iomanip>
#include <sstream>

#include <cmath>
#include <vector>

//librairie rand()%max
#include <cstdlib>
#include <ctime>

#include <chrono>
typedef std::chrono::high_resolution_clock::time_point time_point;

#include <string.h>

#include <fstream>

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720
#define OUTSCREEN_W 500
#define OUTSCREEN_H 100

//structure de position avec x et y
struct Pos{
	int x;
	int y;
};

//fonction de dessin 
void setPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color);
void drawSquare(SDL_Surface *screen, int x, int y, int w, int h, Uint32 color);

//Fonction de detection
char detectAround(Pos a, Pos b);
//distance entre pos1 et pos 2
char range(int x1, int y1, int x2, int y2);


//serpent en lui même
class Snake
{
public:
	Snake(int w, int h);
	~Snake();
	void init();
	void init_after();
	void setMove(int m);
	int getMoveLeft();
	int getMove();
	int get_step();
	void move(int a);
	void addQueue();
	double get_time();
	void set_speed(double v);
	void newFood();
	int get_score();
	bool gameover();
	char* getRangeWall(SDL_Surface *screen = NULL);
	char* getRangeQueue(SDL_Surface *screen = NULL);
	char* getRangeFood(SDL_Surface *screen = NULL); 
	bool collisionQueue(int x, int y);//renvoie true si la position x,y est sur la queue du serpent
	bool collisionWall(int x, int y);//renvoie true si la position x,y est sur un mur ou à l'extérieur
	bool collisionFood(int x, int y);//renvoie true si la position x,y est sur un bonbon
	void draw(SDL_Surface *screen, bool noanimation = 0);//dessine le serpent (fonction appeler ~30 fois par seconde)

private:
	std::vector<Pos> queue;//tableau dynamique qui contient chaque carré du serpent ( Position )
	time_point prevTime, startTime, secondTime; //variable qui sont des points dans le temps pour controler les FPS du jeu
	Pos food; //position de la nouriture ( BONBON)
	int m_w, m_h; //taille d'un carreau en width et en height
	int mouvements, init_move, move_left; //nombre de mouvements de la partie
	char direction, oldDirection, beforeDirection;//direction du serpent
	std::vector<char> map;//tableau dynamique de char (char indique quel type de case) c'est une carte
	int score, max_score, step; //score
	TTF_Font *police;
	bool m_over;
	double vitesse;
	std::ofstream log2;
};

#endif