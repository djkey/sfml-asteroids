#include <SFML/Graphics.hpp>
#include <SFML/Graphics/>
#include<list>
#include<iostream>

using namespace sf;

float DEGTORAD = 0.017453f;

	int win_x = 1000,
		win_y = 1000;


class Entity
{
public:
	float x, y, dx=0, dy=0, R, angle=0;
	bool life;
	std::string name;
	Sprite sprite;

	Entity()
	{
		life = 1;
	}
	

	void settings(Texture &a, int X, int Y, float Angle = 0, int radius = 1)
	{
		settex(a);
		x = X; y = Y;
		angle = Angle;
		R = radius;
	}

	virtual void settex(Texture &a) {};

	virtual void update() {};

	void draw(RenderWindow &win)
	{
		sprite.setPosition(x, y);
		sprite.setRotation(angle+90);
		win.draw(sprite);

		
	}

	virtual ~Entity() {};
};

class Basteroid : public Entity
{
public:
	Basteroid()
	{
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "Basteroid";
	}

	void settex(Texture &a) 
	{
		sprite.setTexture(a);
		sprite.setOrigin(34, 30);//68 60
	}
	void update()
	{
		x += dx;
		y += dy;

		if (x > win_x) x = 0;  if (x < 0) x = win_x;
		if (y > win_y) y = 0;  if (y < 0) y = win_y;
	}

};

class Sasteroid : public Entity
{
public:
	Sasteroid()
	{
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "Sasteroid";
	}
	void settex(Texture &a)
	{
		sprite.setTexture(a);
		sprite.setOrigin(22, 18);//44 36
	}
	void update()
	{
		x += dx;
		y += dy;

		if (x > win_x) x = 0;  if (x < 0) x = win_x;
		if (y > win_y) y = 0;  if (y < 0) y = win_y;
	}

};

class bullet : public Entity
{
public:
	bullet()
	{
		name = "bullet";
	}
	void settex(Texture &a)
	{
		sprite.setTexture(a);
		sprite.setOrigin(6, 6);//13 12
	}
	void  update()
	{
		dx = cos(angle*DEGTORAD) * 6;
		dy = sin(angle*DEGTORAD) * 6;
		 angle+=rand()%5-2; //разброс
		x += dx;
		y += dy;

		if (x > win_y || x<0 || y>win_x || y < 0) life = 0;
	}

};


class player : public Entity
{
public:
	bool thrust;

	player()
	{
		name = "player";
	}
	void settex(Texture &a)
	{
		sprite.setTexture(a);
		sprite.setOrigin(22.5, 24);//45 48
	}
	void update()
	{
		if (thrust)
		{
			dx += cos(angle*DEGTORAD)*0.2;
			dy += sin(angle*DEGTORAD)*0.2;
		}
		else
		{
			dx *= 0.99;
			dy *= 0.99;
		}

		int maxSpeed = 15;
		float speed = sqrt(dx*dx + dy * dy);
		if (speed > maxSpeed)
		{
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx;
		y += dy;

		if (x > win_x) x = 0; 
		if (x < 0) x = win_x;
		if (y > win_y) y = 0; 
		if (y < 0) y = win_y;
	}

};

bool isCollide(Entity *a, Entity *b)
{
	return (b->x - a->x)*(b->x - a->x) +
		(b->y - a->y)*(b->y - a->y) <
		(a->R + b->R)*(a->R + b->R);
}

int chartoint(char* s)
{
	int num = atoi(&s[0]);
	for (int i = 1; i < strlen(s); i++)
	{
		num *= 10;
		num += atoi(&s[i]);
	}
	std::cout << num << std::endl << std::endl;
	return num;
}

/*
win_x win_y		размер экрана
bullet_max		количество пуль
asteroid_count	количество астероидов
*/
int main(int argc, char* argv[])
{
	srand(time(0));

	//сглаживание
	/*ContextSettings settings;
	settings.antialiasingLevel = 8;
	*/
	

	int bullet_count = 0,
		bullet_max = 10,
		asteroid_count = 20;

	//значения извне
	if (argc > 1) win_x = atoi(argv[1]);
	if (argc > 2) win_y = atoi(argv[2]);
	if (argc > 3) asteroid_count = atoi(argv[3]);
	if (argc > 4) bullet_max = atoi(argv[4]);
	


	Image ico;
	ico.loadFromFile("data\avatar.jpg");
	RenderWindow window(VideoMode(win_x, win_y), "SFML_as");
	window.setIcon(64, 64, ico.getPixelsPtr());
	window.setFramerateLimit(60);


	Texture t0, t1, t2, t3, t4, t5, t6, t7;
	t0.loadFromFile("data\background.png");
	t0.setRepeated(true);
	t1.loadFromFile("data\spaceship.png");
	t2.loadFromFile("data\big_asteroid.png");
	t3.loadFromFile("data\small_asteroid.png");
	t4.loadFromFile("data\bullet.png");
	t5.loadFromFile("data\circle.tga");
	t6.loadFromFile("data\enemy.png");
	t7.loadFromFile("data\reticle.png");


	Sprite	background(t0, IntRect(0, 0, win_x, win_y));

	std::list<Entity*> entities;


	//начальная загрузка астероидов
	for (int i = 0, var_x = 0, var_y = 0; i < asteroid_count; i++)
	{
		var_x = rand() % win_x;
		var_y = rand() % win_y;
		if (var_x > win_x / 2 - 100 && var_x < win_x / 2 + 100) var_x -= 200;
		if (var_y > win_y / 2 - 100 && var_y < win_y / 2 + 100) var_y -= 200;

		if (rand() % 2)
		{
			Basteroid *a = new Basteroid();
			a->settings(t2, var_x, var_y, rand() % 360, 25);
			entities.push_back(a);
		}
		else
		{
			Sasteroid *a = new Sasteroid();
			a->settings(t3, rand() % var_x, rand() % var_y, rand() % 360, 25);
			entities.push_back(a);
		}
	}

	player *p = new player();
	p->settings(t1, win_x/2, win_y/2, 0, 20);
	entities.push_back(p);

	float x = 300, y = 300;
	float dx = 0, dy = 0, angle = 0;
	bool thrust = false;



	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}

			if (event.type == Event::KeyPressed)
				if (event.key.code == Keyboard::Space)
				{
					if (bullet_count < bullet_max)
					{
						bullet *b = new bullet();
						b->settings(t4, p->x, p->y, p->angle, 10);
						entities.push_back(b);
						bullet_count++;
					}
				}

		}
		if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 5;
		if (Keyboard::isKeyPressed(Keyboard::Left)) p->angle -= 5;
		if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
		else p->thrust = false;

		//попадание
		for (auto a : entities)
			for (auto b : entities)
			{
				if (a->name == "Basteroid" && b->name == "bullet"|| a->name == "Sasteroid" && b->name == "bullet")
					if (isCollide(a, b))
					{
						a->life = false;
						b->life = false;
						asteroid_count--;
						for (int i = 0; i < ((rand()%2)?2:3)&&a->name == "Basteroid"; i++)
						{
							if (a->R == 15) continue;
							Entity *e = new Sasteroid();
							e->settings(t3, a->x, a->y, rand() % 360, 15);
							entities.push_back(e);
							asteroid_count++;
						}

					}

				if (a->name == "player" && b->name == "Basteroid"|| a->name == "player" && b->name == "Sasteroid")
					if (isCollide(a, b))
					{
						b->life = false;
						
						window.close();
						std::cout << "defeat\n";
						system("pause");
						
					}



			}

		
		
		//удаление
		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity* e = *i;
			e->update();


			if (e->life == false)
			{
				if (e->name == "bullet")
					bullet_count--;
				
				i = entities.erase(i);
				delete e;
			}
			else
			{
				i++;
			}
		}
		
		if (!asteroid_count)
		{
			window.close();
			std::cout << "WIN";
			system("pause");
		}

		window.draw(background);
		for (auto i : entities) i->draw(window);
		window.display();

	}
	
	return 0;
}