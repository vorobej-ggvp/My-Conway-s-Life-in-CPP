#pragma once

#include <vector>
#include <stdexcept>

class Universe
{
	int width, height;
	std::vector<bool> currentGrid;
	std::vector<bool> nextGrid;

public:
	Universe(int w, int h) : width(w), height(h)
	{
		if (width <= 0 || height <= 0) throw std::invalid_argument("Width and height must be positive integers.");
		currentGrid.assign(width * height, false);
		nextGrid.assign(width * height, false);
	}

	void toggleCell(int x, int y)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			currentGrid[y * width + x] = !currentGrid[y * width + x];
		}
	}

	void setCell(int x, int y, bool state)
	{
		if (x >= 0 && x < width && y > 0 && y < height)
		{
			currentGrid[y * width + x] = state;
		}
	}

	void update()
	{
		int w = this->width;
		int h = this->height;

		nextGrid.assign(w * h, false);
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				int n = countNeighbours(x, y);
				int idx = y * w + x;
				if (currentGrid[idx]) nextGrid[idx] = (n == 2 || n == 3);
				else nextGrid[idx] = (n == 3);
			}
		}
		currentGrid = nextGrid;
	}

	int countNeighbours(int px, int py) const
	{
		int count = 0;
		int w = this->width;
		int h = this->height;

		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (i == 0 && j == 0) continue;

				int nx = (px + i + w) % w;
				int ny = (py + j + h) % h;

				if (currentGrid[ny * w + nx]) count++;
			}
		}
		return count;
	}

	void clear()
	{
		std::fill(currentGrid.begin(), currentGrid.end(), false);
	}

	bool isAlive(int x, int y) const
	{
		if (x < 0 || x >= width || y < 0 || y >= height) return false;
		return currentGrid[y * width + x];
	}
	const std::vector<bool>& getState() const
	{
		return currentGrid;
	}
};