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
	//tete du serpent
	Pos p;
	p.x = 4;
	p.y = 5;
	queue.push_back(p);

	//pour controller la direction du serpent
	direction = 1;
	oldDirection = 0;

	//init de la bouffe
	newFood();

	//pour regler la taille des blocs 
	m_w = w;
	m_h = h;

	//pour regler la vitesse du serpent
	prevTime = chrono::high_resolution_clock::now();
}

void Snake::move(int a)
{
	oldDirection = direction;
	switch(a)
	{
		case 0:
			if(oldDirection!=1)
				direction = (char)a;
			break;
		case 1:
			if(oldDirection!=0)
				direction = (char)a;
			break;
		case 2:
			if(oldDirection!=3)
				direction = (char)a;
			break;
		case 3:
			if(oldDirection!=2)
				direction = (char)a; 
			break;
	}
}

void Snake::newFood()
{
	int randomFood = 0;
	do{ 
		randomFood = rand()%(m_w*m_h);
	}while(collisionQueue(randomFood%m_w,int(randomFood/m_w)));

	//nouvelle nouriture
	food.x = randomFood%m_w;
	food.y = int(randomFood/m_w);
}

void Snake::addQueue()
{
	Pos p = queue[queue.size()-1];
	p.x++;
	queue.push_back(p);
}

bool Snake::collisionQueue(int x, int y)
{
	return 0;
}

bool Snake::collisionWall(int x, int y)
{
	return ( x<0 || x>=SCREEN_WIDTH || y<0 || y>=SCREEN_HEIGHT );
}

void Snake::draw(SDL_Surface *screen)
{
	int w = SCREEN_WIDTH/m_w, h = SCREEN_HEIGHT/m_h;

	double run = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()-prevTime).count()/50.0;

	//changement des positions des cases
	if(run>1.0)
	{
		Pos oldPos = queue[0];
		Pos test = queue[0];
		switch(direction)
		{
			case 0:
				test.x--;
				break;
			case 1:
				test.x++;
				break;
			case 2:
				test.y--;
				break;
			case 3:
				test.y++; 
				break;
		}

		//on bloque l'avancement si il y a un mur
		if(!collisionWall(test.x*w,test.y*h))
		{
			queue[0] = test;

			for(int i(0);i<queue.size()-1;i++)
			{
				Pos n = queue[i+1];
				queue[i+1] = oldPos;
				oldPos = n;
			}
		}
		prevTime=chrono::high_resolution_clock::now();
	}

	//dessin des cases
	for(int i(0);i<queue.size();i++)
		drawSquare(screen,queue[i].x*w,queue[i].y*h,w,h,SDL_MapRGB(screen->format,200,200,200));
}