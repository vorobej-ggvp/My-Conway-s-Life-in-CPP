#include <iostream>
#include <deque>
#include <map>
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

void initGrid(sf::VertexArray& grid, int gridSize, float cellSize, float windowSize)
{
	grid.clear();
	grid.setPrimitiveType(sf::PrimitiveType::Lines);
	sf::Color gridColor(40, 40, 40);

	for (int i = 0; i <= gridSize; ++i)
	{
		float pos = i * cellSize;

		grid.append(sf::Vertex({ pos, 0.f }, gridColor));
		grid.append(sf::Vertex({ pos, windowSize }, gridColor));

		grid.append(sf::Vertex({ 0.f, pos }, gridColor));
		grid.append(sf::Vertex({ windowSize, pos }, gridColor));
	}
}

struct Stamp {
	std::string name;
	std::vector<sf::Vector2i> dots;
};

map<int, Stamp> library;

void initLibrary()
{
	library[1] = { "Glider", {{0,-1}, {1,0}, {-1,1}, {0,1}, {1,1}} };

	library[2] = { "Gosper Gun", {
		{24,0}, {22,1}, {24,1}, {12,2}, {13,2}, {20,2}, {21,2}, {34,2}, {35,2},
		{11,3}, {15,3}, {20,3}, {21,3}, {34,3}, {35,3}, {0,4}, {1,4}, {10,4},
		{16,4}, {20,4}, {21,4}, {0,5}, {1,5}, {10,5}, {14,5}, {16,5}, {17,5},
		{22,5}, {24,5}, {10,6}, {16,6}, {24,6}, {11,7}, {15,7}, {12,8}, {13,8}
	} };

	library[3] = { "Simkin Gun", {
		{0,0}, {1,0}, {0,1}, {1,1},
		{7,0}, {8,0}, {7,1}, {8,1},
		{4,3}, {5,3}, {4,4}, {5,4},

		{31,11}, {32,11}, {31,12}, {32,12},

		{20,11}, {21,11}, {21,10}, {22,9}, {21,12}, {22,12}, {22,13},
		{26,9}, {26,10}, {27,10}, {27,11}, {27,12}, {28,11}
	} };

	library[4] = { "35P12H6V0", {
		{0,5}, {1,6}, {2,6}, {3,5}, {4,4}, {4,3}, {4,2}, {3,2}, {2,2}, {1,3},

		{11,1}, {10,0}, {9,0}, {8,0}, {8,1}, {8,2}, {8,3}, {8,4}, {8,5}, {9,6}, {10,6}, {11,5},

		{17,2}, {16,1}, {15,1}, {14,1}, {14,2}, {14,3}, {14,4}, {14,5}, {14,6}, {15,7}, {17,7}, {18,5}, {18,4},
	} };

	library[5] = { "Noah's ark", { 
		{2,-6}, {3,-7}, {3,-5},
		{5,-7}, 
		{5,-4}, {6,-4}, {7,-4}, {6,-5}, 

		{-7,3}, {-6,2}, {-5,3},
		{-7,5},
		{-4,5}, {-4,6}, {-4,7}, {-5,6}
	} };
}

sf::Vector2i getRotatedOffset(sf::Vector2i offset, int rot)
{
	switch (rot)
	{
	case 1:
		return { -offset.y, offset.x };
	case 2:
		return { -offset.x, -offset.y };
	case 3:
		return { offset.y, -offset.x };
	}
	return offset;
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

		sf::VertexArray gridLines;
		initGrid(gridLines, gridSize, cellSize, (float)windowSize);

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

		initLibrary();

		int activeStamp = 0;
		int rotation = 0; // 0: 0°, 1: 90°, 2: 180°, 3: 270°

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

				if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
				{
					if (keyEvent->code >= sf::Keyboard::Key::Num1 && keyEvent->code <= sf::Keyboard::Key::Num5)
					{

						int pressedStamp = static_cast<int>(keyEvent->code) - static_cast<int>(sf::Keyboard::Key::Num1) + 1;

						if (activeStamp == pressedStamp)
						{
							activeStamp = 0;
							cout << "Switched to Pencil" << endl;
						}
						else
						{
							activeStamp = pressedStamp;
							cout << "Selected stamp: " << activeStamp << endl;
						}
					}
					if (keyEvent->code == sf::Keyboard::Key::Num0 || keyEvent->code == sf::Keyboard::Key::Escape)
					{
						activeStamp = 0;
						cout << "Switched to Pencil" << endl;
					}

					if (keyEvent->code == sf::Keyboard::Key::R)
					{
						rotation = (rotation + 1) % 4;
					}
				}

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

							std::cout << "Field Cleared" << endl;
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

							if (x >= 0 && x < gridSize && y >= 0 && y < gridSize)
							{
								if (activeStamp == 0)
								{
									isDrawingMode = true;
									targetState = !world.isAlive(x, y);
									world.setCell(x, y, targetState);
								}
								else
								{
									for (const auto& offset : library[activeStamp].dots)
									{
										sf::Vector2i rotated = getRotatedOffset(offset, rotation);

										int targetX = x + rotated.x;
										int targetY = y + rotated.y;

										if (targetX >= 0 && targetX < gridSize && targetY >= 0 && targetY < gridSize)
										{
											world.setCell(targetX, targetY, true);
										}
									}
								}
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

					std::cout << "Game Over. Cells Alive: " << aliveCount << endl;
				}
				else
				{
					history.push_back(current);
					if (history.size() > maxHistorySize) history.pop_front();
					tracker.pushState(current);
				}
				
				window.setTitle("Score: " + std::to_string(tracker.getScore()) + " | Alive: " + std::to_string(aliveCount));
			}

			window.clear();

			window.setView(worldView);

			window.draw(gridLines);

			for (int y = 0; y < gridSize; ++y)
			{
				for (int x = 0; x < gridSize; ++x)
				{
					if (world.isAlive(x, y)) active.draw(window, x * cellSize, y * cellSize, cellSize);
				}
			}

			if (!running && activeStamp > 0 && showCursor)
			{
				sf::RectangleShape previewCell({ cellSize, cellSize });
				previewCell.setFillColor(sf::Color(255, 255, 255, 100));

				for (const auto& offset : library[activeStamp].dots)
				{
					sf::Vector2i rotated = getRotatedOffset(offset, rotation);

					float px = cellCursor.getPosition().x + (cellSize * rotated.x);
					float py = cellCursor.getPosition().y + (cellSize * rotated.y);

					if (px >= 0 && px < (gridSize * cellSize) && py >= 0 && py < windowSize)
					{
						previewCell.setPosition({ px, py });
						window.draw(previewCell);
					}
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
