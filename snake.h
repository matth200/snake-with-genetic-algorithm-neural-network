#ifndef INCLUDE_SNAKE
#define INCLUDE_SNAKE

#include <SDL/SDL.h>
#include <cmath>
#include <vector>

//librairie rand()%max
#include <cstdlib>
#include <ctime>

#include <chrono>
typedef std::chrono::high_resolution_clock::time_point time_point;

#include <fstream>

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color);
void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color);
void drawSquare(SDL_Surface *screen, int x, int y, int w, int h, Uint32 color);

struct Pos{
	int x;
	int y;
};

class Snake
{
public:
	Snake(int w, int h);
	void move(int a);
	void addQueue();
	void newFood();
	bool collisionQueue(int x, int y);
	bool collisionWall(int x, int y);
	void draw(SDL_Surface *screen);

private:
	std::vector<Pos> queue;
	time_point prevTime;
	Pos food;
	int m_w, m_h;
	char direction, oldDirection;
	std::vector<char> map;

	std::ofstream log2;
};

#endif