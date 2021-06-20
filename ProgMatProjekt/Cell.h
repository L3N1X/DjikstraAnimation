#pragma once
#include<iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>

//Pomak po x osi jer polje nije velicine ekrana
#define xOffset 420.0f

int rnd(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

struct Cell
{
	int x{ 0 }, y{ 0 };
	int number{ 0 };
	bool isPath = false;
	bool isWall = false;
	bool isTarget = false;
	Cell()
	{
		square.setPosition({ 20.0f * x + xOffset, 20.0f * y });
		square.setFillColor(sf::Color::Green);
		square.setSize({ 20.0f,20.0f });
	}
	Cell(int x, int y)
	{
		this->x = x;
		this->y = y;
		square.setPosition({ 20.0f * x + xOffset, 20.0f * y });
		square.setSize({ 20.0f,20.0f });
	}
	void draw(sf::RenderWindow& window)
	{
		if (isTarget)
		{
			square.setFillColor(sf::Color::White);
			window.draw(square);
		}	
		//else if (!isWall && !isPath) //Nemoj crtati
		//{
		//}
		else if (!isWall && isPath)
		{
			square.setFillColor(sf::Color(255,rnd(0,50),rnd(120,255)));
			window.draw(square);
		}
		else if (isWall && !isPath)
		{
			int rndNum = rnd(50, 100);
			square.setFillColor(sf::Color(rndNum, rndNum, rndNum));
			window.draw(square);
		}
	}
private:
	sf::RectangleShape square;
};