#include "snake.h"
using namespace std;

//function to draw

void setPixel(SDL_Surface *screen, int x, int y, Uint32 color)
{
	if(x>=0&&x<SCREEN_WIDTH+OUTSCREEN_W&&y>=0&&y<SCREEN_HEIGHT+OUTSCREEN_H)
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
		double diffX = x2-x1, diffY = y1-y2;
		for(int i(0);i<abs(diffX);i++)
		{
			setPixel(screen,x1+i,y1-diffY/diffX*i,color);
		}
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

char detectAround(Pos a, Pos b)
{
	if(a.x+1==b.x&&a.y==b.y)
		return 1;
	else if(a.x-1==b.x&&a.y==b.y)
		return 0;
	else if(a.x==b.x&&a.y-1==b.y)
		return 2;
	else if(a.x==b.x&&a.y+1==b.y)
		return 3;
	else
		return -1;
}


char range(int x1, int y1, int x2, int y2)
{
	return  char(int(sqrt(pow(x2-x1,2)+pow(y2-y1,2))));
}

//class Snake

Snake::Snake(int w, int h)
{
	//initialiser une seule fois de tous le jeu
	log2.open("../log/log2.txt");
	police = TTF_OpenFont("../ressources/pixel_font.ttf",32);
	max_score = 0;
	m_over = 0;
	
	//pour regler la taille des blocs 
	m_w = w;
	m_h = h;

	init_after();

	//init pour le jeu qui est appelé à chaque fois que le joueur perds
	init();
}
Snake::~Snake()
{
	TTF_CloseFont(police);
}

void Snake::init()
{
	queue.clear();

	//tete du serpent
	Pos p;
	p.x = 29;
	p.y = 29;
	queue.push_back(p);

	//vitesse initialiser
	vitesse = 1.0;

	//5 bloc de base avec la tete
	addQueue();
	addQueue();
	addQueue();
	addQueue();

	//pour controller la direction du serpent
	direction = 3;
	beforeDirection = 3;
	oldDirection = 0;

	//tableau dynamique qui contient les données de la map
	map.clear();
	map.resize(m_w*m_h,-1);

	//init de la bouffe
	newFood();

	//pour regler la vitesse du serpent
	prevTime = chrono::high_resolution_clock::now();
}

void Snake::init_after()
{
	//tempss
	startTime = chrono::high_resolution_clock::now();	
	secondTime = chrono::high_resolution_clock::now();

	//Score
	score = 0;

	//step
	step = 0;

	mouvements = 0;
}

void Snake::setMove(int m)
{
	init_move = m;
	move_left = m;
}

int Snake::getMoveLeft()
{
	return move_left;
}

int Snake::getMove()
{
	return mouvements;
}

void Snake::move(int a)
{
	oldDirection = direction;

	//empecher qu'il se retourne dans son corps
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
	}while(collisionQueue(randomFood%m_w,int(randomFood/m_w))||(queue[0].x==randomFood%m_w&&queue[0].y==int(randomFood/m_w)));

	move_left = init_move+score*100;
	if(move_left>500)
		move_left=500;

	//nouvelle nouriture
	food.x = randomFood%m_w;
	food.y = int(randomFood/m_w);
}

char* Snake::getRangeWall(SDL_Surface *screen)
{
	unsigned char *data = new unsigned char[8];
	memset(data,0,8);
	Pos p = queue[0];
	
	//droite
	bool collision = 0;
	unsigned char index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x+i, y= p.y;
		collision = collisionWall(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
		}
		else if(screen!=NULL){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,100,100));
		}
	}
	data[1] = index;

	//gauche
	collision = 0;
	index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x-i, y= p.y;
		collision = collisionWall(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
		}
		else if(screen!=NULL){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,100,100));
		}
	}
	data[0] = index;

	//en bas
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&!collision;i++)
	{
		int x = p.x, y= p.y+i;
		collision = collisionWall(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
		}
		else if(screen!=NULL){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,100,100));
		}
	}
	data[3] = index;

	//en haut
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&!collision;i++)
	{
		int x = p.x, y= p.y-i;
		collision = collisionWall(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
		}
		else if(screen!=NULL){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,100,100));
		}
	}
	data[2] = index;

	//diagonal en haut à gauche
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&i<m_w&&!collision;i++)
	{
		collision = collisionWall(p.x-i,p.y-i);
		if(collision)
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
	}
	data[4] = index;

	//diagonal en haut à droite
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&i<m_w&&!collision;i++)
	{
		collision = collisionWall(p.x+i,p.y-i);
		if(collision)
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
	}
	data[5] = index;

	//diagonal en bas à droite
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&i<m_w&&!collision;i++)
	{
		collision = collisionWall(p.x+i,p.y+i);
		if(collision)
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
	}
	data[6] = index;

	//diagonal en bas à gauche
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&i<m_w&&!collision;i++)
	{
		collision = collisionWall(p.x-i,p.y+i);
		if(collision)
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
	}
	data[7] = index;

	return (char*)data;
}

char* Snake::getRangeQueue(SDL_Surface *screen)
{
	unsigned char *data = new unsigned char[8];
	memset(data,0,8);

	//position de la tete
	Pos p = queue[0];
	
	//droite
	bool collision = 0, stop = 0;
	unsigned char index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x+i, y= p.y;
		collision = collisionQueue(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[1] = index;

	//gauche
	collision = 0, stop = 0;
	index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x-i, y= p.y;
		collision = collisionQueue(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[0] = index;

	//en bas
	collision = 0, stop = 0;
	index = 0;
	for(int i(1);i<m_h&&!collision;i++)
	{
		int x = p.x, y= p.y+i;
		collision = collisionQueue(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[3] = index;

	//en haut
	collision = 0, stop = 0;
	index = 0;
	for(int i(1);i<m_h&&!collision;i++)
	{
		int x = p.x, y= p.y-i;
		collision = collisionQueue(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[2] = index;

	//diagonal en haut à gauche
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&i<m_w&&!collision;i++)
	{
		collision = collisionQueue(p.x-i,p.y-i);
		if(collision)
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
	}
	data[4] = index;

	//diagonal en haut à droite
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&i<m_w&&!collision;i++)
	{
		collision = collisionQueue(p.x+i,p.y-i);
		if(collision)
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
	}
	data[5] = index;

	//diagonal en bas à droite
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&i<m_w&&!collision;i++)
	{
		collision = collisionQueue(p.x+i,p.y+i);
		if(collision)
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
	}
	data[6] = index;

	//diagonal en bas à gauche
	collision = 0;
	index = 0;
	for(int i(1);i<m_h&&i<m_w&&!collision;i++)
	{
		collision = collisionQueue(p.x-i,p.y+i);
		if(collision)
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
	}
	data[7] = index;

	return (char*)data;
}

char* Snake::getRangeFood(SDL_Surface *screen)
{
	unsigned char *data = new unsigned char[8];
	memset(data,0,8);

	//position de la tete
	Pos p = queue[0];
	
	//droite
	bool collision = 0, stop = 0;
	unsigned char index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x+i, y= p.y;
		collision = collisionFood(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[1] = index;

	//gauche
	collision = 0;
	stop = 0;
	index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x-i, y= p.y;
		collision = collisionFood(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[0] = index;

	//en bas
	collision = 0;
	stop = 0;
	index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x, y= p.y+i;
		collision = collisionFood(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[3] = index;

	//en haut
	collision = 0;
	stop = 0;
	index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x, y= p.y-i;
		collision = collisionFood(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[2] = index;

	//diagonal en haut à gauche
	collision = 0;
	stop = 0;
	index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x-i, y= p.y-i;
		collision = collisionFood(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[4] = index;

	//diagonal en haut à droite
	collision = 0;
	stop = 0;
	index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x+i, y= p.y-i;
		collision = collisionFood(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[5] = index;

	//diagonal en bas à droite
	collision = 0;
	stop = 0;
	index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x+i, y= p.y+i;
		collision = collisionFood(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[6] = index;

	//diagonal en bas à gauche
	collision = 0;
	stop = 0;
	index = 0;
	for(int i(1);i<m_w&&!collision;i++)
	{
		int x = p.x-i, y= p.y+i;
		collision = collisionFood(x,y);
		if(collision){
			index = (unsigned char)(255-double(i-1)/m_w*255.0);
			stop = 1;
		}
		else if(screen!=NULL&&!stop){
			drawSquare(screen,x*SCREEN_WIDTH/m_w,y*SCREEN_HEIGHT/m_h,SCREEN_WIDTH/m_w,SCREEN_HEIGHT/m_h,SDL_MapRGB(screen->format,100,200,100));
		}
	}
	data[7] = index;

	return (char*)data;
}

void Snake::addQueue()
{
	//on suit la direction de la queue pour l'ajout du nouveau bout
	Pos p = queue[queue.size()-1];
	Pos a = queue[queue.size()-2];
	switch(detectAround(a,p))
	{
		case -1:
			p.x++;
			break;
		case 0:
			p.x--;
			break;
		case 1:
			p.x++;
			break;
		case 2:
			p.y--;
			break;
		case 3:
			p.y++;
			break;

	}

	queue.push_back(p);
}

bool Snake::collisionQueue(int x, int y)
{
	return (map[x+y*m_w]==0);
}

bool Snake::collisionWall(int x, int y)
{
	//on renvoit la condition
	return ( x<0 || x>=m_w || y<0 || y>=m_h );
}

bool Snake::collisionFood(int x, int y)
{
	return x==food.x&&y==food.y;
}

int Snake::get_score()
{
	return score;
}

bool Snake::gameover()
{
	bool over = m_over;
	m_over = 0;
	return over;
}

void Snake::set_speed(double v)
{
	if(v>0)
		vitesse = v;
} 

double Snake::get_time()
{
	return double(chrono::duration_cast<chrono::milliseconds>(secondTime-startTime).count()/1000.0);
}

int Snake::get_step()
{
	return step;
}

void Snake::draw(SDL_Surface *screen, bool noanimation)
{
	int w = SCREEN_WIDTH/m_w, h = SCREEN_HEIGHT/m_h;

	//variable de la vitesse du serpent
	double run = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()-prevTime).count()/50.0*vitesse;

	//on met run à 1 pour que les cases avancent tous le temps de 1 à chaque appel de fonction
	if(noanimation)
		run = 1;

	//changement du max_score
	if(score>max_score)
		max_score = score;

	//fooddd
	if(!noanimation)
		drawSquare(screen,food.x*w,food.y*h,w,h,SDL_MapRGB(screen->format,255,25,25));

	//changement des positions des cases
	if(run>=1.0)
	{
		step++;
		Pos test = queue[0];

		//changements de direction
		if(beforeDirection!=direction){
			move_left--;
			mouvements++;
		}
		beforeDirection = direction;

		//on applique le mouvement par rapport à la direction voulu
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

		Pos oldPos = queue[0];

		//on bloque l'avancement si il y a un mur
		if(!collisionWall(test.x,test.y)&&!collisionQueue(test.x,test.y)&&move_left>0)
		{
			queue[0] = test;
			//effacement de la map
			map.clear();
			map.resize(m_w*m_h,-1);
			
			for(int i(0);i<queue.size();i++)
			{
				if(i<queue.size()-1)
				{
					Pos n = queue[i+1];
					queue[i+1] = oldPos;
					oldPos = n; 
				}
				//enregistrement des états dans la map
				map[queue[i].x+queue[i].y*m_w] = 0;
			}
		}
		//si le joueur perds
		else
		{
			//affichage de la cause
			if(!noanimation)
			{
				if(move_left<=0)
					log2 << "mort par mouvement trop intense" << endl;
				else if(collisionQueue(test.x,test.y))
					log2 << "mort par collision de la Queue" << endl;
				else if(collisionWall(test.x,test.y))
					log2 << "mort par collision sur un mur" << endl;
			}

			init();
			m_over = 1;
		}

		//detection de la bouffe
		if(collisionFood(queue[0].x,queue[0].y))
		{
			newFood();
			addQueue();
			score++;
		}

		prevTime=chrono::high_resolution_clock::now();
	}

	//si il n'y a pas d'animation on s'en fout
	if(!noanimation)
	{
		//dessin des cases
		for(int i(0);i<queue.size();i++)
			drawSquare(screen,queue[i].x*w,queue[i].y*h,w,h,SDL_MapRGBA(screen->format,200,200,200,100));

		//AFFICHAGE DU SCORE

		//on met dans un stringstream pour avoir que 3 décimals
		//stringstream streamTime;
		//streamTime << std::fixed << std::setprecision(3) << chrono::duration_cast<chrono::milliseconds>(secondTime-startTime).count()/1000.0; 

		SDL_Surface *texte = TTF_RenderText_Solid(police,(string("score : ")+to_string(score)+" ("+to_string(max_score)+")    step : "+to_string(get_step())).c_str(),SDL_Color({255,255,255}));
		SDL_Rect pos;
		pos.y = SCREEN_HEIGHT+25;
		pos.x = 25;
		SDL_BlitSurface(texte,NULL,screen,&pos);
		SDL_FreeSurface(texte);

	}
}