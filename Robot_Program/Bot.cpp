#include "Bot.h"

const int WALL = 0;
const int PASSAGE = 1;
const int GOAL = 2;
const int VISITONCE = 4;
const int VISITTWICE = 5;

Bot::Bot(sf::RenderWindow &window)
{
	viewRect.setFillColor(sf::Color::Magenta);
	viewRect.setSize(sf::Vector2f(1, 20));
	bodyRect.setFillColor(sf::Color::Green);
	bodyRect.setSize(sf::Vector2f(20, 20));
	currentDirection = UP;
}

Bot::~Bot()
{
	
}

void Bot::Update()
{
	bodyRect.setPosition(bodyPos);
	viewRect.setPosition(bodyPos.x + 10, bodyPos.y + 10);

	switch (currentDirection)
	{
		case UP:	viewRect.setRotation(180.0f);		break;
		case DOWN:	viewRect.setRotation(0.0f);			break;
		case LEFT:	viewRect.setRotation(90.0f);		break;
		case RIGHT:	viewRect.setRotation(270.0f);		break;
	}
}


void Bot::SendCells(std::vector<Cell>&cells)
{
	this->cells = cells;
}

int Bot::GetBotPosition()
{
	return currentCell;
}

void Bot::SetBotPosition(int currentCell)
{
	this->currentCell = currentCell;
	bodyPos = cells[this->currentCell].pos;
}

bool Bot::FindPath(int value, std::vector<Cell>&cells)
{
	std::vector <Cell> adjacentCells;
	adjacentCells.clear();
	
	if (cells[currentCell].type == PASSAGE)
		cells[currentCell].type = VISITONCE;
	else
		cells[currentCell].type = VISITTWICE;


	if (currentCell == value)
	{
		return true;
	}
	else
	{
		//Get adjacent cells

		adjacentCells.push_back(cells[currentCell - 1]);	//up
		adjacentCells.push_back(cells[currentCell + 1]);	//down
		adjacentCells.push_back(cells[currentCell - 40]);	//left
		adjacentCells.push_back(cells[currentCell + 40]);	//right

		for (unsigned int i = 0; i < adjacentCells.size(); i++)
		{
			if (adjacentCells[i].type != WALL)
			{
				if (adjacentCells[i].type < VISITONCE) //New Cell
				{
					UpdateDirection(i + 1);
					SetBotPosition(adjacentCells[i].id);
					return false;
				}
			}
		}

		//If no new cells were discovered dead end etc..
		//Retrace back to find a new path
		for (unsigned int i = 0; i < adjacentCells.size(); i++)
		{
			if (adjacentCells[i].type != WALL)
			{
				if (adjacentCells[i].type < VISITTWICE) //New Cell
				{
					cells[currentCell].type = VISITTWICE;
					UpdateDirection(i + 1);
					SetBotPosition(adjacentCells[i].id);
					return false;
				}
			}
		}

		//If we're at a snag where we have no choice but to go on a square a third time
		for (unsigned int i = 0; i < adjacentCells.size(); i++)
		{
			if (adjacentCells[i].type != WALL)
			{
				if (adjacentCells[i].type == VISITTWICE && i == currentDirection) 
				{
					UpdateDirection(i + 1);
					SetBotPosition(adjacentCells[i].id);
					return false;
				}
			}
		}



		
	}
}

void Bot::UpdateDirection(int dir)
{
	switch (dir)
	{
		case 1:		
			currentDirection = UP;
			if (cells[currentCell-1].type == PASSAGE)
				SetBotPosition(currentCell - 1);
			break;
		case 2:		
			currentDirection = DOWN;
			if (cells[currentCell + 1].type == PASSAGE)
				SetBotPosition(currentCell + 1);
			break;
		case 3:		
			currentDirection = LEFT;
			if (cells[currentCell - 40].type == PASSAGE)
				SetBotPosition(currentCell - 40);
			break;
		case 4:		
			currentDirection = RIGHT;
			if (cells[currentCell + 40].type == PASSAGE)
				SetBotPosition(currentCell + 40);
			break;

	}
}

void Bot::Draw(sf::RenderWindow &window)
{
	
	window.draw(bodyRect);
	window.draw(viewRect);
}