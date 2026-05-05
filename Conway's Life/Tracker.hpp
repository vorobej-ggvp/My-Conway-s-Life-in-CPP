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
			//history.push_back(state);
			if (history.size() > 2) history.erase(history.begin());
		}
	}
	int getScore() const { return score; }
};