#include <iostream>
#include <deque>
#include <SFML/Graphics.hpp>
#include "Universe.h"
#include "Tracker.hpp"
#include "Cell.h"

using namespace std;

void updateButtonText(sf::Text& text, const sf::RectangleShape& button, const string& newString)
{
	text.setString(newString);

	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.getCenter());

	text.setPosition(button.getGlobalBounds().getCenter());
}

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

		float currentZoom = 1.0f;
		sf::View worldView(sf::FloatRect({ 0.f, 0.f }, { (float)windowSize, (float)windowSize }));
		worldView.setViewport(sf::FloatRect({ 0.f, 0.f }, { 1.f, (float)windowSize / (windowSize + 50.f) }));

		sf::RectangleShape btn({ 120.f, 40.f });
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

		sf::RectangleShape clearBtn({ 100.f, 40.f });
		clearBtn.setFillColor(sf::Color(150, 50, 50));
		clearBtn.setPosition({ windowSize - 110.f, windowSize + 10.f });

		sf::Text clearText(font, "Clear");
		clearText.setCharacterSize(20);
		clearText.setOrigin(clearText.getLocalBounds().getCenter());
		clearText.setPosition(clearBtn.getGlobalBounds().getCenter());

		sf::Text coordsText(font, "X: 0, Y: 0");
		coordsText.setCharacterSize(16);
		coordsText.setFillColor(sf::Color(255, 255, 255, 180));
		coordsText.setPosition({ 10.f, 10.f });

		sf::RectangleShape cellCursor({ cellSize, cellSize });
		cellCursor.setFillColor(sf::Color::Transparent);
		cellCursor.setOutlineColor(sf::Color(255, 255, 255, 150));
		cellCursor.setOutlineThickness(-1.f);

		bool showCursor = false;

		//world.toggleCell(50, 50);
		//world.toggleCell(51, 50);
		//world.toggleCell(52, 50);
		//world.toggleCell(52, 49);
		//world.toggleCell(51, 48);

		deque<vector<bool>> history;
		const size_t maxHistorySize = 4;

		sf::Clock clock;
		float timer = 0;
		float delay = .1f;

		bool isDrawingMode = false;
		bool targetState = true;

		while (window.isOpen())
		{
			float time = clock.getElapsedTime().asSeconds();
			clock.restart();
			timer += time;

			while (const optional event = window.pollEvent())
			{
				if (event->is<sf::Event::Closed>()) window.close();

				if (const auto* wheelEvent = event->getIf<sf::Event::MouseWheelScrolled>())
				{
					if (wheelEvent->wheel == sf::Mouse::Wheel::Vertical)
					{
						float zoomAmount = 1.1f;
						if (wheelEvent->delta > 0 && currentZoom > 0.1f)
						{
							worldView.zoom(1.f / zoomAmount);
							currentZoom /= zoomAmount;
						}
						if (wheelEvent->delta < 0 && currentZoom < 2.0f)
						{
							worldView.zoom(zoomAmount);
							currentZoom *= zoomAmount;
						}
					}
				}

				if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
				{
					if (mouseEvent->button == sf::Mouse::Button::Left)
					{
						sf::Vector2f uiMousePos = window.mapPixelToCoords(mouseEvent->position, window.getDefaultView());

						sf::Vector2f mousePos = window.mapPixelToCoords(mouseEvent->position, worldView);

						if (clearBtn.getGlobalBounds().contains(uiMousePos))
						{
							running = false;
							world.clear();
							tracker.clear();
							history.clear();

							btn.setFillColor(sf::Color::Blue);
							updateButtonText(btnText, btn, "Start");

							window.setTitle("Score: 0 | Alive: 0");

							cout << "Field Cleared" << endl;
						}

						if (btn.getGlobalBounds().contains(uiMousePos))
						{

							if (!running)
							{
								if (btnText.getString() == "Start")
								{
									tracker.clear();
									history.clear();

									window.setTitle("Score: 0 | Alive: " + to_string(count(world.getState().begin(), world.getState().end(), true)));
								}

								running = true;
								btn.setFillColor(sf::Color(100, 100, 100));
								updateButtonText(btnText, btn, "Pause");
							}
							else
							{
								running = false;
								btn.setFillColor(sf::Color(200, 100, 0));
								updateButtonText(btnText, btn, "Resume");
							}
						}

						else if (!running && mousePos.y < windowSize)
						{
							int x = static_cast<int>(mousePos.x / cellSize);
							int y = static_cast<int>(mousePos.y / cellSize);

							if (x >= 0 && x < gridSize && y > 0 && y < gridSize)
							{
								isDrawingMode = true;
								targetState = !world.isAlive(x, y);
								world.setCell(x, y, targetState);
							}
						}
					}
				}

				if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>())
				{
					if (mouseEvent->button == sf::Mouse::Button::Left)
					{
						isDrawingMode = false;
					}
				}

				if (const auto* mouseEvent = event->getIf<sf::Event::MouseMoved>())
				{

					sf::Vector2f worldPos = window.mapPixelToCoords(mouseEvent->position, worldView);

					int x = static_cast<int>(worldPos.x / cellSize);
					int y = static_cast<int>(worldPos.y / cellSize);

					if (x >= 0 && x < gridSize && y >= 0 && y < gridSize && worldPos.y < windowSize)
					{
						showCursor = true;
						cellCursor.setPosition({ x * cellSize, y * cellSize });

						coordsText.setString("X: " + to_string(x) + "Y: " + to_string(y));
					}
					else
					{
						showCursor = false;
						coordsText.setString("");
					}


					if (isDrawingMode && !running)
					{
						sf::Vector2f mousePos = window.mapPixelToCoords(mouseEvent->position, worldView);

						int x = static_cast<int>(mousePos.x / cellSize);
						int y = static_cast<int>(mousePos.y / cellSize);

						if (x >= 0 && x < gridSize && y > 0 && y < gridSize)
						{
							world.setCell(x, y, targetState);
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

					window.setTitle("Game over! Score: " + to_string(tracker.getScore()) + " | Alive: " + to_string(aliveCount));

					btn.setFillColor(sf::Color::Blue);
					updateButtonText(btnText, btn, "Start");

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

			window.setView(worldView);

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

			if (showCursor && !running) window.draw(cellCursor);

			window.setView(window.getDefaultView());

			window.draw(btn);
			window.draw(btnText);
			window.draw(clearBtn);
			window.draw(clearText);

			if (coordsText.getString() != "") window.draw(coordsText);

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
