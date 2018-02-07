#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <time.h>
#include <string>
#include <algorithm>
#include "Cell.h"
#include "Bot.h"

int windowWidth = 800;
int windowHeight = 800;
sf::Color black = sf::Color(0, 0, 0);
sf::Color white = sf::Color(255, 255, 255);

const int WALL = 0;
const int PASSAGE = 1;
const int GOAL = 3;
const int VISITONCE = 4;
const int VISITTWICE = 5;



//		[ 0  40  80 ]
//		[ 1  41  81 ]
//		[ 2  42  82 ]
// total cells = 1600



void MarkCell(int value, std::vector<Cell>&cells, std::vector<Cell>&frontier_cells);
void AddFrontierCell(int value, std::vector<Cell>&cells, std::vector<Cell>&frontier_cells);
std::vector <Cell> InitialiseMaze(Bot *bot, sf::RenderWindow &window);
std::vector<Cell> GetNeighbours(int value, std::vector<Cell>&cells);
void SetGoal(Cell cell, sf::RenderWindow &window);
void DrawGrid(std::vector<Cell>&cells, sf::RenderWindow &window);
bool CheckFrontierCells(int cellid, std::vector<Cell>&frontier_cells);
void ShowPath(int startCell, int endCell, std::vector<Cell>&cells);




int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight+100), "My window");

	Bot *bot = new Bot(window);

	bool newMaze = true;
	bool searchPath = false;

	sf::Font font;

	if (!font.loadFromFile("arial.ttf"))
	{
		std::cout << "font not loaded";
	}

	sf::Clock timer;
	sf::Text timerText;
	sf::Text blockText;

	timerText.setFont(font);
	timerText.setCharacterSize(24);
	timerText.setFillColor(sf::Color::White);
	timerText.setPosition(sf::Vector2f(10.0f, 810.0f));

	blockText.setFont(font);
	blockText.setCharacterSize(24);
	blockText.setFillColor(sf::Color::White);
	blockText.setPosition(sf::Vector2f(10.0f, 850.0f));



	std::vector<Cell> cells;

	int startCell;
	int goalCell;
	int cellsChecked;

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					newMaze = true;
				}

				if (event.key.code == sf::Keyboard::Up)
				{
					bot->UpdateDirection(1);
				}

				if (event.key.code == sf::Keyboard::Down)
				{
					bot->UpdateDirection(2);
				}

				if (event.key.code == sf::Keyboard::Left)
				{
					bot->UpdateDirection(3);
				}

				if (event.key.code == sf::Keyboard::Right)
				{
					bot->UpdateDirection(4);
				}
				
			}

			if (event.type == sf::Event::MouseButtonReleased)
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);

				for (unsigned int i = 0; i < cells.size(); i++)
				{
					if (mousePos.x >= cells[i].pos.x && mousePos.x < cells[i].pos.x + 20 &&
						mousePos.y >= cells[i].pos.y && mousePos.y < cells[i].pos.y + 20 && cells[i].type == PASSAGE)
					{
						//Begin looking for goal
						startCell = bot->GetBotPosition();
						goalCell = i;
						cells[goalCell].type = GOAL;
						searchPath = true;
						cellsChecked = 0;
						sf::Clock timer;
						 
					}	
				}
				for (unsigned int i = 0; i < cells.size(); i++)
				{
					if (cells[i].type > PASSAGE && cells[i].type != GOAL)
						cells[i].type = PASSAGE;
				}
			}
		}

		if (newMaze == true)
		{
			cells.clear();
			cells = InitialiseMaze(bot, window);
			newMaze = false;
		}

		if (searchPath == true)
		{
			//start timer
			

			sf::Time timeElapsed = timer.getElapsedTime();

			timerText.setString("Time taken: "+std::to_string(timeElapsed.asSeconds()));
			blockText.setString("Cells Checked " + std::to_string(cellsChecked));

			if (bot->FindPath(goalCell, cells) == true)
			{
				searchPath = false;
				ShowPath(startCell, goalCell, cells);	
			}
			else
			{
				cellsChecked++;
			}
			sf::sleep(sf::seconds(0.01));
		}
		
		

		// clear the window with black color
		window.clear(sf::Color::Black);

		bot->Update();
		
		DrawGrid(cells, window);
		bot->Draw(window);

		window.draw(timerText);
		window.draw(blockText);

		// end the current frame
		window.display();
  	}

	delete bot;
	cells.clear();

	return 0;
}

std::vector <Cell> InitialiseMaze(Bot *bot, sf::RenderWindow &window)
{
	std::vector<Cell>cells;
	std::vector<Cell>frontier_cells;

	int count = 0;

	for (float x = 0.0f; x < windowWidth; x += 20.0f)
	{
		for (float y = 0.0f; y < windowHeight; y += 20.0f)
		{
			Cell cell;
			cell.id = count;
			cell.type = WALL;
			cell.pos = sf::Vector2<float>(x, y);
			cells.push_back(cell);
			count++;
		}
	}

	bot->SendCells(cells);

	srand(time(0));

	int value = rand() % cells.size();

	MarkCell(value, cells, frontier_cells);
	bot->SetBotPosition(value);

	while (frontier_cells.size() > 0)
	{
		//Select random frontier
		int frontierValue = rand() % frontier_cells.size();
		int frontierCellId = frontier_cells[frontierValue].id;

		//DrawCell(cells[frontierCellId], window);

		//Get neighbours of random frontier
		std::vector<Cell> neighbours = GetNeighbours(frontierCellId, cells);

		//pick a random neighbour, if start there will only be one neighbour

		int neighbourValue = rand() % neighbours.size();
		int neighbourCellId = neighbours[neighbourValue].id;
		//Work out where this neighbour is (Up, Down Left or right)
		if (neighbourCellId == frontierCellId + 2)	//Below frontier
			cells[frontierCellId + 1].type = PASSAGE;
		else if (neighbourCellId == frontierCellId - 2)	//Above frontier
			cells[frontierCellId - 1].type = PASSAGE;
		else if (neighbourCellId == frontierCellId + 80)	//Right frontier
			cells[frontierCellId + 40].type = PASSAGE;
		else if (neighbourCellId == frontierCellId - 80)	//Left frontier
			cells[frontierCellId - 40].type = PASSAGE;


		MarkCell(frontierCellId, cells, frontier_cells);

		frontier_cells.erase(frontier_cells.begin() + frontierValue);

		DrawGrid(cells, window);
		window.display();

	}

	bot->SendCells(cells);

	return cells;

}

std::vector<Cell> GetNeighbours(int value, std::vector<Cell>&cells)
{
	std::vector<Cell> neighbours;

	if (value + 2 < 1600 && cells[value].pos.y <= 1560.0f)
	{
		if (cells[value + 2].type == PASSAGE)
			neighbours.push_back(cells[value + 2]);
	}
	if (value - 2 > 0 && cells[value].pos.y >= 40.0f)
	{
		if (cells[value - 2].type == PASSAGE)
			neighbours.push_back(cells[value - 2]);
	}

	if (value + 80 < 1600 && cells[value].pos.x <= 1560.0f)
	{
		if (cells[value + 80].type == PASSAGE)
			neighbours.push_back(cells[value + 80]);
	}

	if (value - 80 > 0 && cells[value].pos.x >= 40.0f)
	{
		if (cells[value - 80].type == PASSAGE)
			neighbours.push_back(cells[value - 80]);
	}

	return neighbours;
}

void MarkCell(int value, std::vector<Cell>&cells, std::vector<Cell>&frontier_cells)
{

	cells[value].type = PASSAGE;

	if (value + 2 < 1600 && cells[value].pos.y <= 1540.0f)											//BELOW
		AddFrontierCell(value + 2, cells, frontier_cells);
	if (value - 2 > 0 && cells[value].pos.y >= 40.0f)																	//ABOVE
		AddFrontierCell(value - 2, cells, frontier_cells);
	if (value + 80 < 1600 && cells[value].pos.x <= 1540.0f)																//RIGHT
		AddFrontierCell(value + 80, cells, frontier_cells);
	if (value - 80 > 0 && cells[value].pos.x >= 40.0f)																		//LEFT
		AddFrontierCell(value - 80, cells, frontier_cells);
}

void AddFrontierCell(int value, std::vector<Cell>&cells, std::vector<Cell>&frontier_cells)
{
	if(cells[value].pos.x >= 20.0f && cells[value].pos.y >= 20.0f && cells[value].pos.x < float(windowWidth-20.0f) && cells[value].pos.y < float(windowHeight-20.0f)
		&& cells[value].type != PASSAGE && !CheckFrontierCells(cells[value].id, frontier_cells))
	{
		frontier_cells.push_back(cells[value]);
	}
}

bool CheckFrontierCells(int cellid, std::vector<Cell>&frontier_cells)
{
	for (unsigned int i = 0; i < frontier_cells.size(); i++)
	{
		if (frontier_cells[i].id == cellid)
			return true;
	}

	return false;
}


void DrawGrid(std::vector<Cell>&cells, sf::RenderWindow &window)
{

	for (unsigned int i = 0; i < cells.size(); i++)
	{
		sf::RectangleShape rectangle;
		
		if (cells[i].type == GOAL)
			rectangle.setFillColor(sf::Color::Magenta);
		else if (cells[i].type == VISITONCE)
			rectangle.setFillColor(sf::Color::Cyan);
		else if (cells[i].type == VISITTWICE)
			rectangle.setFillColor(sf::Color::Red);
		else if (cells[i].type == PASSAGE)
			rectangle.setFillColor(sf::Color::White);
		else
			rectangle.setFillColor(sf::Color::Black);

		rectangle.setPosition(cells[i].pos);
		rectangle.setSize(sf::Vector2f(20, 20));

		
		window.draw(rectangle);
		
	}

	//sf::sleep(sf::milliseconds(0.001f));

}

void ShowPath(int startCell, int endCell, std::vector<Cell>&cells)
{
	std::vector<Cell>adjacentCells;
	int currentCell = endCell;
	int previousCell = 1601;
	bool visitCellFound = false;

	while (CheckFrontierCells(startCell, adjacentCells) == false)
	{
		adjacentCells.clear();
		visitCellFound = false;

		adjacentCells.push_back(cells[currentCell - 1]);
		adjacentCells.push_back(cells[currentCell + 1]);
		adjacentCells.push_back(cells[currentCell - 40]);
		adjacentCells.push_back(cells[currentCell + 40]);

		for (unsigned int i = 0; i < adjacentCells.size(); i++)
		{
			if (adjacentCells[i].type == VISITONCE && adjacentCells[i].id != previousCell)
			{
				previousCell = currentCell;
				currentCell = adjacentCells[i].id;
				visitCellFound = true;
				break;
			}
		}

		if (!visitCellFound)
		{
			for (unsigned int i = 0; i < adjacentCells.size(); i++)
			{
				if (adjacentCells[i].type == VISITTWICE && adjacentCells[i].id != previousCell)
				{
					previousCell = currentCell;
					currentCell = adjacentCells[i].id;
					cells[adjacentCells[i].id].type = VISITONCE;
					break;
				}
			}
		}
	}

	

}
