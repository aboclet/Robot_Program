#ifndef __CELL_H__
#define __CELL_H__


#include <SFML/Graphics.hpp>

struct Cell
{
	int id;
	int type;
	sf::Vector2<float> pos;
};

#endif