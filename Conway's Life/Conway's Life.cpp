#include <iostream>
#include <deque>
#include <SFML/Graphics.hpp>
#include "Universe.h"
#include "Tracker.hpp"
#include "Cell.h"

using namespace std;

int main()
{
	try
	{
		const int windowSize = 800;
		const int gridSize = 100;
		const float cellSize = static_cast<float>(windowSize) / gridSize;

		const unsigned int uWindowSize = 800;
		const unsigned int uBottomPanel = 60;

		sf::VideoMode vm({ uWindowSize, uWindowSize + uBottomPanel });
		sf::RenderWindow window(vm, "Conway's Life C++");
		Universe world(gridSize, gridSize);
		EvolutionTracker<vector<bool>> tracker;

		ActiveCell active;
		BackgroundCell empty;
		bool running = false;

		sf::RectangleShape btn(sf::Vector2f(120, 40));
		btn.setPosition(sf::Vector2f(10, uWindowSize + 10));
		btn.setFillColor(sf::Color::Blue);

		sf::Font font;
		if (!font.openFromFile("font.ttf"))
		{
			throw runtime_error("Failed to load font: 'font'.ttf");
		}

		sf::Text btnText(font, "Start");
		btnText.setCharacterSize(20);
		btnText.setFillColor(sf::Color::White);

		sf::FloatRect textRect = btnText.getLocalBounds();
		btnText.setOrigin(textRect.getCenter());

		btnText.setPosition(btn.getGlobalBounds().getCenter());

		world.toggleCell(50, 50);
		world.toggleCell(51, 50);
		world.toggleCell(52, 50);
		world.toggleCell(52, 49);
		world.toggleCell(51, 48);

		deque<vector<bool>> history;
		const size_t maxHistorySize = 4;

		sf::Clock clock;
		float timer = 0;
		float delay = .1f;

		while (window.isOpen())
		{
			float time = clock.getElapsedTime().asSeconds();
			clock.restart();
			timer += time;

			while (const optional event = window.pollEvent())
			{
				if (event->is<sf::Event::Closed>()) window.close();

				if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
				{
					if (mouseEvent->button == sf::Mouse::Button::Left)
					{
						float clickX = static_cast<float>(mouseEvent->position.x);
						float clickY = static_cast<float>(mouseEvent->position.y);

						if (btn.getGlobalBounds().contains({ clickX, clickY }))
						{
							running = !running;

							if (running)
							{
								btn.setFillColor(sf::Color(100, 100, 100));
								btnText.setString("Pause");
							}
							else
							{
									btn.setFillColor(sf::Color(200, 100, 0));
									btnText.setString("Resume");
												
							}

							btnText.setOrigin(btnText.getLocalBounds().getCenter());
							btnText.setPosition(btn.getGlobalBounds().getCenter());
						}

						else if (!running && clickY < windowSize)
						{
							int gridX = static_cast<int>(clickX / cellSize);
							int gridY = static_cast<int>(clickY / cellSize);
							world.toggleCell(gridX, gridY);
										
						}
					}
				}
			}

			if (running && timer > delay)
			{
				timer = 0;
				world.update();

				const vector<bool>& current = world.getState();
				size_t aliveCount = count(current.begin(), current.end(), true);

				bool isStagnant = false;

				for (const auto& past : history)
				{
					if (current == past)
					{
						isStagnant = true;
						break;
					}
				}

				if (isStagnant || aliveCount == 0)
				{
					running = false;
					btn.setFillColor(sf::Color::Blue);
					btnText.setString("Start");
					btnText.setOrigin(btnText.getLocalBounds().getCenter());
					btnText.setPosition(btn.getGlobalBounds().getCenter());

					cout << "Game Over. Cells Alive: " << aliveCount << endl;
				}
				else
				{
					history.push_back(current);
					if (history.size() > maxHistorySize) history.pop_front();
					tracker.pushState(current);
				}

				if (running)
				{
					history.push_back(current);
					if (history.size() > maxHistorySize) history.pop_front();
				}
				
				window.setTitle("Score: " + std::to_string(tracker.getScore()) + " | Alive: " + std::to_string(aliveCount));
			}

			window.clear();

			sf::VertexArray gridLines(sf::PrimitiveType::Lines);
			sf::Color gridColor(40, 40, 40);

			for (int i = 0; i <= gridSize; ++i)
			{
				float pos = i * cellSize;

				gridLines.append(sf::Vertex({ pos, 0 }, gridColor));
				gridLines.append(sf::Vertex({ pos, (float)(windowSize) }, gridColor));

				gridLines.append(sf::Vertex({ 0, pos}, gridColor));
				gridLines.append(sf::Vertex({ (float)(windowSize), pos }, gridColor));
			}
			window.draw(gridLines);

			for (int y = 0; y < gridSize; ++y)
			{
				for (int x = 0; x < gridSize; ++x)
				{
					if (world.isAlive(x, y)) active.draw(window, x * cellSize, y * cellSize, cellSize);
				}
			}

			window.draw(btn);
			window.draw(btnText);
			window.display();
		}
	}

	catch (const exception& e)
	{
		cerr << "Fatal Error: " << e.what() << endl;
		return -1;
	}
	return 0;
}
