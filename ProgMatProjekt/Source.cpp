#include <iostream>
#include <ctime>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
//#include <SFML/Audio.hpp>
//WINDOWS MORA ICI POSLIJE SFML-a
#include <Windows.h>

#include "Game.h"
#include "SpriteButton.h"
bool isMouseInWindow(sf::RenderWindow& window)
{
	sf::Vector2f windowSize{ window.getSize() };
	sf::Vector2f mousePos{ sf::Mouse::getPosition() };
	if (mousePos.x >= 0.0f && mousePos.x <= windowSize.x && mousePos.y >= 0.0f && mousePos.y <= windowSize.y)
		return true;
	return false;
}
bool isMouseInGrid(sf::RenderWindow& window)
{
	sf::Vector2f mousePos{ sf::Mouse::getPosition() };
	if (mousePos.x >= 420.0 && mousePos.x < 1500.0f
		&& mousePos.y >= 0.0f && mousePos.y <= 1080.f)
		return true;
	return false;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "CURSED DJIKSTRA", sf::Style::Fullscreen);
	window.setFramerateLimit(60);

	srand(time(nullptr));
	Game* game = new Game;
	stringstream ss;
	ss << "SRC:\tSet source" << endl << endl 
		<< "DST:\tSet destination" << endl << endl
		<< "CLR:\tClear grid" << endl << endl
		<< "GO:\tStart animation" << endl << endl
		<< "Left Mouse click:\tPlace wall, source or destination" << endl << endl
		<< "Right Mouse click:\tRemove wall" << endl << endl
		<< "You can't remove source or destination" << endl << endl
		<< "unless you clear the grid";
	string helpString{ ss.str() };

	sf::RectangleShape signature;
	sf::Texture signatureTexture;
	if (!signatureTexture.loadFromFile("\Textures\\signature.png"))
	{
		std::cout << "ERROR: Loading signature.png failed" << std::endl;
		return 1;
	}
	signature.setTexture(&signatureTexture);
	signature.setSize({ 100.0f,100.0f });
	signature.setPosition({ 1800.0f,1000.0f });

	sf::Font font;
	if (!font.loadFromFile("\Fonts\\Bates Shower.ttf"))
	{
		std::cout << "ERROR: Loading resource file failed" << std::endl;
		return 1;
	}

	sf::Texture background;
	if (!background.loadFromFile("\Textures\\background.jpg"))
	{
		std::cout << "ERROR: Loading resource file failed" << std::endl;
		return 1;
	}
	sf::Texture backgroundCursed;
	if (!backgroundCursed.loadFromFile("\Textures\\backgroundCursed.jpg"))
	{
		std::cout << "ERROR: Loading resource file failed" << std::endl;
		return 1;
	}

	sf::RectangleShape gridBackground;
	gridBackground.setSize({ 1080.0f,1080.0f });
	gridBackground.setPosition({ 420.0f,0.0f });
	gridBackground.setTexture(&background);

	sf::Text text;
	text.setCharacterSize(90);
	text.setFont(font);
	text.setPosition({ 70.0f,95.0f });
	text.setString(game->pathLengthString());

	sf::Text helpText;
	helpText.setCharacterSize(30);
	helpText.setFont(font);
	helpText.setPosition({ 30.0f,500.0f });
	helpText.setString(helpString);

	sf::Texture left;
	if (!left.loadFromFile("\Textures\\left.jpg"))
		return 1;
	sf::RectangleShape leftBorder;
	leftBorder.setSize({ 420.0f, 1080.0f });
	leftBorder.setPosition({ 0.0f,0.0f });
	leftBorder.setTexture(&left);

	sf::Texture right;
	if (!right.loadFromFile("\Textures\\right.jpg"))
		return 1;
	sf::RectangleShape rightBorder;
	rightBorder.setSize({ 420.0f, 1080.0f });
	rightBorder.setPosition({ 420.0f + 1080.0f,0.0f });
	rightBorder.setTexture(&right);

	TwoStateButton srcButton({ 1650.0f,90.0f }, 1.5f, "\Textures\\srcUnactive.png", "\Textures\\srcActive.png");
	srcButton.setHoverTextures("\Textures\\srcUnactiveHover.png", "\Textures\\srcActiveHover.png");
	srcButton.setSounds("\Sounds\\clok.wav", "\Sounds\\fartSound.ogg");
	TwoStateButton dstButton({ 1650.0f,90.0f + 150.0f }, 1.5f, "\Textures\\dstUnactive.png", "\Textures\\dstActive.png");
	dstButton.setHoverTextures("\Textures\\dstUnactiveHover.png", "\Textures\\dstActiveHover.png");
	dstButton.setSounds("\Sounds\\clok.wav", "\Sounds\\fartSound.ogg");
	TwoStateButton goButton({ 1650.0f,90.0f + 300.0f }, 1.5f, "\Textures\\go.png", "\Textures\\go.png");
	goButton.setHoverTextures("\Textures\\goHover.png", "\Textures\\goHover.png");
	goButton.setSounds("\Sounds\\clok.wav", "\Sounds\\fartSound.ogg");
	TwoStateButton clrButton({ 1650.0f,90.0f + 450.0f }, 1.5f, "\Textures\\clr.png", "\Textures\\clr.png");
	clrButton.setHoverTextures("\Textures\\clrHover.png", "\Textures\\clrHover.png");
	clrButton.setSounds("\Sounds\\clok.wav", "\Sounds\\fartSound.ogg");
	TwoStateButton helpButton({ 1650.0f,90.0f + 600.0f }, 1.5f, "\Textures\\help.png", "\Textures\\help.png");
	helpButton.setHoverTextures("\Textures\\helpHover.png", "\Textures\\helpHover.png");
	helpButton.setSounds("\Sounds\\clok.wav", "\Sounds\\fartSound.ogg");
	TwoStateButton exitButton({ 1650.0f,90.0f + 750.0f }, 1.5f, "\Textures\\exit.png", "\Textures\\exit.png");
	exitButton.setHoverTextures("\Textures\\exitHover.png", "\Textures\\exitHover.png");
	exitButton.setSounds("\Sounds\\clok.wav", "\Sounds\\fartSound.ogg");
	
	bool calc = false;
	bool showHelp = false;
	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				window.close();
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (!dstButton.is_active())
				{
					srcButton.checkActiveStatus(window);
					if(srcButton.isMouseOver(window))
						srcButton.playClickSound();
				}
				if (!srcButton.is_active())
				{
					dstButton.checkActiveStatus(window);
					if(dstButton.isMouseOver(window))
						dstButton.playClickSound();
				}
				if (clrButton.isMouseOver(window))
				{
					game->resetGrid();
					gridBackground.setTexture(&background);
					clrButton.playClickSound();
				}
				if (exitButton.isMouseOver(window))
				{
					exitButton.playClickSound();
					window.close();
				}
				if (helpButton.isMouseOver(window))
				{
					helpButton.playClickSound();
				}
				helpButton.checkActiveStatus(window);
			}
		}
		window.clear(sf::Color::Black);
		/*sf::Time dt = clock.restart();*/
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && srcButton.is_active()
			&& !dstButton.is_active() && isMouseInGrid(window))
		{
				game->setSourceCell(window);
				srcButton.set_unactive();
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !srcButton.is_active()
			&& !dstButton.is_active() && isMouseInGrid(window))
		{
			game->setWallCell(window);
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !srcButton.is_active()
			&& dstButton.is_active() && isMouseInGrid(window))
		{
			game->setDestinationCell(window);
			dstButton.set_unactive();
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !srcButton.is_active()
			&& !dstButton.is_active())
		{
			game->setEmptyCell(window);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			game->resetGrid();
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
			goButton.isMouseOver(window) && !srcButton.is_active()
			&& !dstButton.is_active())
		{
			goButton.playClickSound();
			calc = true;
		}
		if (calc)
		{
			game->calculatePath();
			if (game->pathExists())
				gridBackground.setTexture(&background);
			else if(game->src() != -1 && game->dst() != -1)
				gridBackground.setTexture(&backgroundCursed);
			calc = false;
		}
		window.draw(gridBackground);
		game->drawGrid(window);
		window.draw(leftBorder);
		window.draw(rightBorder);
		if (helpButton.is_active())
			window.draw(helpText);
		text.setString(game->pathLengthString());
		window.draw(text);
		srcButton.drawTo(window);
		dstButton.drawTo(window);
		goButton.drawTo(window);
		clrButton.drawTo(window);
		exitButton.drawTo(window);
		helpButton.drawTo(window);
		window.draw(signature);
		window.display();
	}
	string url{ "https://www.youtube.com/watch?v=iik25wqIuFo" };
	ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	delete game;
	return 0;
}