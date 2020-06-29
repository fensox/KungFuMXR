#pragma once
#include <random>

class RandomNumEasy {
private:
	static std::ranlux48 rneGen;

public:
	static int getRandInt(int low, int high) {
		std::uniform_int_distribution<int>  uniform_range(low, high);
		return uniform_range(rneGen);
	}
};