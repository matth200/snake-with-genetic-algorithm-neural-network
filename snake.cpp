#include "snake.h"
using namespace std;

//function to draw

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color)
{
	if(x>=0&&x<SCREEN_WIDTH&&y>=0&&y<SCREEN_HEIGHT)
		*((Uint32*)(screen->pixels)+x+y*screen->w) = color;
}

void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color)
{
	if(x1==x2){
		const int diff = y2-y1;
		for(int i(0);i<abs(diff);i++)
		{
			setPixel(screen,x1,y1+diff/abs(diff)*i,color);
		}
	}else if(y1==y2){
		const int diff = x2-x1;
		for(int i(0);i<abs(diff);i++)
		{
			setPixel(screen,x1+diff/abs(diff)*i,y1,color);
		}
	}
	else{

	}
}

void drawSquare(SDL_Surface *screen, int x, int y, int w, int h, Uint32 color)
{
	for(int iw(0);iw<w;iw++)
	{
		for(int ih(0);ih<h;ih++)
		{
			setPixel(screen,x+iw,y+ih,color);
		}
	}
}

//class

Snake::Snake(int w, int h)
{
	Pos p;
	p.x = 4;
	p.y = 5;
	queue.push_back(p);

	m_w = w;
	m_h = h;

	prevTime = chrono::high_resolution_clock::now();
}

void Snake::move(int a)
{
	direction = (char)a;
}

void Snake::addFood()
{

}

void Snake::draw(SDL_Surface *screen)
{
	double run = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()-prevTime).count()/1000.0*30.0;
	if(direction==0||direction==2)
		run=-run;
	drawSquare(screen,queue[0].x*30.0+((direction<=1)?run:0),queue[0].y*30.0+((direction>1)?run:0),30,30,SDL_MapRGB(screen->format,255,25,25));
	
	if(abs(run/30.0)>1.0)
	{
		switch(direction)
		{
			case 0:
				queue[0].x--;
				break;
			case 1:
				queue[0].x++;
				break;
			case 2:
				queue[0].y--;
				break;
			case 3:
				queue[0].y++; 
		}
		prevTime=chrono::high_resolution_clock::now();
	}
}