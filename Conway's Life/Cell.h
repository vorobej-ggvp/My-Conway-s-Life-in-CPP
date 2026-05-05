#pragma once

#include <SFML/Graphics.hpp>

// Base class for all entities that can be drawn on the screen
class DrawableEntity
{
public:
	virtual ~DrawableEntity() = default;
	virtual void draw(sf::RenderWindow& window, float x, float y, float size) = 0;
};

class GameCell : public DrawableEntity
{
protected:
	sf::Color color;
public:
	GameCell(sf::Color color) : color(color) {}
};

class ActiveCell : public GameCell
{
public:
	ActiveCell() : GameCell(sf::Color::Green) {}
	void draw(sf::RenderWindow& window, float x, float y, float size) override
	{
		sf::RectangleShape rect(sf::Vector2f(size - 1.0f, size - 1.0f));
		rect.setPosition(sf::Vector2f(x, y));
		rect.setFillColor(color);
		window.draw(rect);
	}
};

class BackgroundCell : public GameCell
{
public:
	BackgroundCell() : GameCell(sf::Color::Black) {}
	void draw(sf::RenderWindow& window, float x, float y, float size) override
	{
		sf::RectangleShape rect(sf::Vector2f(size - 1.0f, size - 1.0f));
		rect.setPosition(sf::Vector2f(x, y));
		rect.setFillColor(color);
		window.draw(rect);
	}
};