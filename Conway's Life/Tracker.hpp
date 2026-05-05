#pragma once

#include <vector>

template <typename T>
class EvolutionTracker
{
	std::vector<T> history;
	int score = 0;

public:
	void pushState(const std::vector<bool>& state)
	{
		if (history.empty() || state != history.back())
		{
			score++;
			if (history.size() > 2) history.erase(history.begin());
		}
	}

	void clear()
	{
		score = 0;
		history.clear();
	}

	int getScore() const { return score; }
};