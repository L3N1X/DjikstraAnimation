#pragma once
#pragma warning(disable : 4996)

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class TwoStateButton
{
private:

	bool isActive{ false };
	bool changedState{ false };
	bool isHovered{ false };

	sf::Vector2f position;
	sf::RectangleShape button;

	sf::Texture activeTexture;
	sf::Texture activeHoverTexture;

	sf::Texture unactiveTexture;
	sf::Texture unactiveHoverTexture;

	sf::SoundBuffer hoverBuffer;
	sf::Sound hoverSound;

	sf::SoundBuffer clickBuffer;
	sf::Sound clickSound;

public:
	TwoStateButton(sf::Vector2f pos, float scale, std::string unactiveTextureFileName, std::string activeTextureFilename)
		: position{pos}
	{
		if (!activeTexture.loadFromFile(activeTextureFilename) || !unactiveTexture.loadFromFile(unactiveTextureFileName))
		{
			throw std::exception("ERROR: LOADING TEXTURES FAILED");
		}
		button.setSize(sf::Vector2f(scale*100.0f, scale*100.0f));
		button.setPosition(position);
		button.setTexture(&unactiveTexture);
		activeHoverTexture = activeTexture;
		unactiveHoverTexture = unactiveTexture;
	}
	void setHoverTextures(std::string unactiveHoverTextureFilename, std::string activeHoverTextureFilename)
	{
		if (!activeHoverTexture.loadFromFile(activeHoverTextureFilename) || !unactiveHoverTexture.loadFromFile(unactiveHoverTextureFilename))
		{
			throw std::exception("ERROR: LOADING HOVER TEXTURES FAILED");
		}
	}
	//drawTo function checks hover states also
	void drawTo(sf::RenderWindow& window)
	{
		checkChangedState(window); //Pogledaj dali se promjenilo stanje
		if (changedState)  //Ako se promijenilo stanje
		{
			if (isHovered) //Ako je mis dosao na gumb
			{
				if (isActive) //Ako je gumb aktivan
					button.setTexture(&activeHoverTexture);
				else if(!isActive)//Ako gumb nije aktivan
					button.setTexture(&unactiveHoverTexture);
				hoverSound.play();
			}
			else if(!isHovered) //Ako je mis otisao s gumba
			{
				if (isActive) //Ako je gumb aktivan
					button.setTexture(&activeTexture);
				else if(!isActive)//Ako gumb nije aktivan
					button.setTexture(&unactiveTexture);
			}
			changedState = false; //Stanje smo promijenili, mozemo resetirati
		}
		window.draw(button);
	}
	bool isMouseOver(sf::RenderWindow& window)
	{
		float mouseX = sf::Mouse::getPosition(window).x;
		float mouseY = sf::Mouse::getPosition(window).y;

		float btnPosX = button.getPosition().x;
		float btnPosY = button.getPosition().y;

		float btnxPosWidth = button.getPosition().x + button.getLocalBounds().width;
		float btnyPosHeight = button.getPosition().y + button.getLocalBounds().height;

		if (mouseX < btnxPosWidth && mouseX > btnPosX && mouseY < btnyPosHeight && mouseY > btnPosY)
		{
			return true;
		}
		return false;
	}
	void checkActiveStatus(sf::RenderWindow& window)
	{
		//Dodaj ovdje klik, a ne u main
		if (isMouseOver(window))
		{
			isActive = !isActive;
			if (isActive && isHovered)
				button.setTexture(&activeHoverTexture);
			else if (isActive && !isHovered)
				button.setTexture(&activeTexture);
			else if (!isActive && isHovered)
				button.setTexture(&unactiveHoverTexture);
			else if (!isActive && !isHovered)
				button.setTexture(&unactiveTexture);
		}
	}
	void checkChangedState(sf::RenderWindow& window)
	{
		if (isMouseOver(window) && !isHovered) //Ako mis dode na gumb a nije prije bio
		{
			isHovered = true;
			changedState = true;
		}
		else if (!isMouseOver(window) && isHovered) //Ako gumb nije na misu a prije JE bio
		{
			isHovered = false;
			changedState = true;
		}
	}
	bool is_active()
	{
		return isActive;
	}
	void set_unactive()
	{
		isActive = false;
		if (!isActive && isHovered)
			button.setTexture(&unactiveHoverTexture);
		else if (!isActive && !isHovered)
			button.setTexture(&unactiveTexture);
	}
	void setSounds(std::string hoverSoundFilename, std::string clickSoundFilename)
	{
		if (!hoverBuffer.loadFromFile(hoverSoundFilename) || !clickBuffer.loadFromFile(clickSoundFilename))
			throw std::exception("ERROR: LOADING SOUND FAILED");
		hoverSound.setBuffer(hoverBuffer);
		clickSound.setBuffer(clickBuffer);
	}
	void playClickSound()
	{
		clickSound.play();
	}
};
