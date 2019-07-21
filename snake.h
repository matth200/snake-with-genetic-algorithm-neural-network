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
#define OUTSCREEN_W 400
#define OUTSCREEN_H 100

struct Pos{
	int x;
	int y;
};

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color);
void drawSquare(SDL_Surface *screen, int x, int y, int w, int h, Uint32 color);
char detectAround(Pos a, Pos b);
char range(int x1, int y1, int x2, int y2);


class Snake
{
public:
	Snake(int w, int h);
	~Snake();
	void init();
	void init_time();
	void move(int a);
	void addQueue();
	double get_time();
	void set_speed(double v);
	void newFood();
	int get_score();
	bool gameover();
	char* getRangeWall();
	char* getRangeQueue();
	char* getRangeFood();
	bool collisionQueue(int x, int y);
	bool collisionWall(int x, int y);
	bool collisionFood(int x, int y);
	void draw(SDL_Surface *screen, bool pause);

private:
	std::vector<Pos> queue;
	time_point prevTime, startTime, secondTime;
	Pos food;
	int m_w, m_h;
	char direction, oldDirection;
	std::vector<char> map;
	int score, max_score;
	TTF_Font *police;
	bool m_over;
	double vitesse;
	std::ofstream log2;
};

#endif