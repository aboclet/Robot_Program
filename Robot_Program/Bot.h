#include <SFML/Graphics.hpp>
#include <vector>
#include "Cell.h"

class Bot
{
private:

	enum Direction{ UP, DOWN, LEFT, RIGHT };

	sf::RectangleShape viewRect;
	sf::RectangleShape bodyRect;
	sf::Vector2f bodyPos;
	sf::Vector2f viewPos;
	sf::RenderWindow wind;
	Direction currentDirection;
	int currentCell;
	std::vector<Cell> cells;
	std::vector<Cell> path;

public:

	Bot(sf::RenderWindow &window);
	~Bot();
	void Update();
	void UpdateDirection(int dir);
	void SendCells(std::vector<Cell>&cells);
	void SetBotPosition(int currentCell);
	int GetBotPosition();
	bool FindPath(int value, std::vector<Cell>&cells);
	void Draw(sf::RenderWindow &window);

};