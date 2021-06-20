#pragma once
#include<iostream>
#include<queue>
#include<algorithm>
#include<vector>
#include<set>
#include<map>
#include<list>
#include<functional>
#include<string>
#include<sstream>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Audio.hpp>

#include"Cell.h"

#define xOffset 420.0f
//Integer Pair, node i weight wt
#define iPair std::pair<int,int>
//Infinity
#define INF 999
//Adjency Matrix rows and columns
#define N 2916
//Grid rows and columns
#define n 54 
#define node first
#define wt second

using namespace std;

void setNbr(vector<iPair> v[], int parrentNode, int nbrNode)
{
	v[parrentNode][nbrNode].wt = 1;
	v[nbrNode][parrentNode].wt = 1;
}
void setWall(vector<iPair> v[], int parrentNode, int nbrNode)
{
	v[parrentNode][nbrNode].wt = INF;
	v[nbrNode][parrentNode].wt = INF;
}
int float_to_index(float& pos)
{
	int index;
	index = 10.0f + pos; //33.4
	index = (int)(pos - (int)pos % 10); //30.4 - 0.4
	index = pos / 10; //3 -- To je index koji trazimo
	index = index / 2; //Jer imamo rezolucija / 20 kvadratica
	return index;
}
class Game
{
private:
	vector<vector<Cell>> grid;
	map<int, Cell*> gridMap;
	vector<iPair> v[N];
	list<int> path;
	sf::Vector2f offset{ -420.0f, 0.0f };
	sf::SoundBuffer heheBuffer;
	sf::Sound heheSound;
	int srcCell{ -1 };
	int dstCell{ -1 };
	int pathLength{ 0 };
	bool isSrcPlaced{ false };
	bool isDstPlaced{ false };
	bool isPath{ true };
private:
	//Methods
	void setCell(vector<iPair> v[], Cell& parrentNode, function<void(vector<iPair>[], int, int)> func)
	{
		int x = parrentNode.x;
		int y = parrentNode.y;
		int i = parrentNode.number;
		if (y != 0 && y != n - 1 && x != 0 && x != n - 1)
		{
			func(v, i, i - n);
			func(v, i, i - 1);
			func(v, i, i + 1);
			func(v, i, i + n);
		}
		else if (y == 0 && x == 0) //Gornji lijevi kut
		{
			func(v, i, i + 1);
			func(v, i, i + n);
		}
		else if (y == 0 && x == n - 1) //gornji desni kut
		{
			func(v, i, i - 1);
			func(v, i, i + n);
		}
		else if (y == n - 1 && x == n - 1) //Donji desni kut
		{
			func(v, i, i - 1);
			func(v, i, i - n);
		}
		else if (y == n - 1 && x == 0) //Donji lijevi kut
		{
			func(v, i, i + 1);
			func(v, i, i - n);
		}
		else if (y == 0) //Gornji rub
		{
			func(v, i, i - 1);
			func(v, i, i + 1);
			func(v, i, i + n);
		}
		else if (y == n - 1) //Donji rub
		{
			func(v, i, i - n);
			func(v, i, i - 1);
			func(v, i, i + 1);
		}
		else if (x == 0) //Lijevi rub
		{
			func(v, i, i + 1);
			func(v, i, i + n);
			func(v, i, i - n);
		}
		else if (x == n - 1) //Desni rub
		{
			func(v, i, i - 1);
			func(v, i, i + n);
			func(v, i, i - n);
		}
	}
	list<int> DijkstraGetShortestPathTo(int vertex, const vector<int>& previous)
	{
		list<int> path;
		for (; vertex != -1; vertex = previous[vertex])
			path.push_front(vertex);
		return path;
	}
	list<int> shortestPath(vector<iPair> v[], int sourceNode, int targetNode)
	{
		//Ovdje je label za node u .second, zato jer priority_queue se sortira po .frist u pair<T,T>
		priority_queue<iPair, vector<iPair>, greater<iPair>> pq; //Priority queue sa iPair koji radi na vector<iPair>
		vector<int> dist(N, INF); //Sve udaljenosti ima N elemenata incijaliziranih na INF (999)

		pq.push(make_pair(0, sourceNode)); //Ubaci source node sa udaljenosti 0
		dist[sourceNode] = 0;

		vector<int> previous;
		previous.clear();
		previous.resize(v->size(), -1);

		while (!pq.empty())
		{
			int currNode{ pq.top().second };
			pq.pop();
			//Sve node koje su povezane sa currNode, zato je v[currNode]
			for (auto& i : v[currNode])
			{
				int nbrNode{ i.first };
				int nbrWeight{ i.second };
				if (dist[nbrNode] > dist[currNode] + nbrWeight)
				{
					dist[nbrNode] = dist[currNode] + nbrWeight;
					previous[nbrNode] = currNode;
					pq.push(make_pair(dist[nbrNode], nbrNode));
				}
			}
		}
		/*cout << "Distance from source:" << endl;
		for (auto i{ 0 }; i != N; ++i)
		{
			cout << "Node " << i << "\t" << dist[i] << endl;
		}*/
		list<int> path = DijkstraGetShortestPathTo(targetNode, previous);
		if (path.size() == 1)
		{
			heheSound.play();
			isPath = false;
		}
		else
			isPath = true;
		pathLength = dist[targetNode];
		return path;
	}
	void initializeGrid(vector<vector<Cell>>& grid)
	{
		for (int y = 0; y != n; y++)
		{
			vector<Cell> temp;
			for (int x = 0; x != n; x++)
			{
				temp.emplace_back(x, y);
			}
			grid.push_back(temp);
		}
	}
	void initializeGridMap(vector<vector<Cell>>& grid, map<int, Cell*>& gridMap)
	{
		int index{ 0 };
		for (int y = 0; y != n; y++)
		{
			for (int x = 0; x != n; x++)
			{
				gridMap[index] = &grid[y][x];
				gridMap[index]->number = index;
				index++;
			}
		}
	}
	void initializeMatrix(vector<iPair> v[])
	{
		for (int i = 0; i != N; i++)
		{
			for (int j = 0; j != N; j++)
			{
				v[i].push_back(make_pair(j, INF));
				if (i == j)
					v[i][j].wt = 0;
			}
		}
	}
	void printMatrix(vector<iPair> v[])
	{
		for (int i = 0; i != N; i++)
		{
			for (int j = 0; j != N; j++)
			{
				cout << v[i][j].wt << "\t";
			}
			cout << endl;
		}
	}
	void gridToMatrix(vector<iPair> v[], vector<vector<Cell>>& grid)
	{
		for (int y = 0; y < n; y++)
		{
			for (int x = 0; x < n; x++)
			{
				setCell(v, grid[y][x], setNbr);
			}
		}
	}
	void printPathInfo(list<int>& path, map<int, Cell*>& gridMap)
	{
		cout << endl << "Path cells: " << endl;
		for (auto it = path.begin(); it != path.end(); ++it)
		{
			cout << *it << " ";
		}
		cout << endl << endl;
		cout << "Coordinates: " << endl;
		for (auto it = path.begin(); it != path.end(); ++it)
		{
			cout << "Cell: " << *it << "\t" << gridMap[*it]->x << " " << gridMap[*it]->y << endl;;
		}
	}
public:
	Game()
	{
		//Create a grid of cells
		initializeGrid(grid);
		//Create a map of pointers to cells in grid
		initializeGridMap(grid, gridMap);
		//Create Adjency matrix and fill with grid info
		initializeMatrix(v);
		gridToMatrix(v, grid);
		if (!heheBuffer.loadFromFile("\Sounds\\hehe.wav"))
			cout << "ERROR: can't load sound" << endl;
		heheSound.setBuffer(heheBuffer);
	}
	~Game()
	{
	}
	bool isMouseInGrid(sf::RenderWindow& window)
	{
		sf::Vector2f mousePos{ sf::Mouse::getPosition() };
		if (mousePos.x >= 420.0 && mousePos.x < 1500.0f
			&& mousePos.y >= 0.0f && mousePos.y <= 1080.f)
			return true;
		return false;
	}
	void setWallCell(sf::RenderWindow& window)
	{
		if (isMouseInGrid(window))
		{
			sf::Vector2f mousePos{ sf::Mouse::getPosition() };
			mousePos = mousePos + offset;
			if (grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].number != srcCell
				&& grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].number != dstCell)
			{
				setCell(v, grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)], setWall);
				grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].isWall = true;
				grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].isPath = false;
			}
		}
	}
	void setEmptyCell(sf::RenderWindow& window)
	{
		if (isMouseInGrid(window))
		{
			sf::Vector2f mousePos{ sf::Mouse::getPosition() };
			mousePos = mousePos + offset;
			if (grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].number != srcCell
				&& grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].number != dstCell)
			{
				setCell(v, grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)], setNbr);
				grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].isWall = false;
			}
		}
	}
	void setSourceCell(sf::RenderWindow& window)
	{
		if (isMouseInGrid(window) && srcCell == -1)
		{
			sf::Vector2f mousePos{ sf::Mouse::getPosition() };
			mousePos = mousePos + offset;
			srcCell = grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].number;
			grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].isPath = true;
		}
	}
	void setDestinationCell(sf::RenderWindow& window)
	{
		if (isMouseInGrid(window) && dstCell == -1)
		{
			sf::Vector2f mousePos{ sf::Mouse::getPosition() };
			mousePos = mousePos + offset;
			dstCell = grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].number;
			grid[float_to_index(mousePos.y)][float_to_index(mousePos.x)].isTarget = true;
		}
	}
	void drawGrid(sf::RenderWindow& window)
	{
		if (!path.empty())
		{
			for (int y = 0; y != n; y++)
			{
				gridMap[path.front()]->isPath = true;
				for (int x = 0; x != n; x++)
				{
					grid[y][x].draw(window);
				}
			}
			//gridMap[path.front()]->isPath = false;
			path.pop_front();
		}
		else
		{
			for (int y = 0; y != n; y++)
			{
				for (int x = 0; x != n; x++)
				{
					grid[y][x].draw(window);
				}
			}
		}
	}
	void calculatePath()
	{
		if(srcCell != -1 && dstCell != -1)
			path = shortestPath(v, srcCell, dstCell);
	}
	void resetGrid()
	{
		//isSrcPlaced = false;
		//isDstPlaced = false;
		srcCell = -1;
		dstCell = -1;
		pathLength = 0;
		for (int y = 0; y != n; y++)
		{
			for (int x = 0; x != n; x++)
			{
				setCell(v, grid[y][x], setNbr);
				grid[y][x].isPath = false;
				grid[y][x].isWall = false;
				grid[y][x].isTarget = false;
			}
		}
	}
	bool isSourcePlaced()
	{
		if (srcCell != -1)
			return true;
		return false;
	}
	bool isDestinationPlaced()
	{
		if (dstCell != -1)
			return true;
		return false;
	}
	bool pathExists()
	{
		return isPath;
	}
	int src()
	{
		return srcCell;
	}
	int dst()
	{
		return dstCell;
	}
	string pathLengthString()
	{
		stringstream ss;
		if (pathLength == INF)
			ss << "X\t\t\t\t\t\t  NOCANDO";
		else
			ss << pathLength;
		return ss.str();
	}
};

