#include <random>
#include <iostream>
#include <chrono>
#include <string>


float function(float x) {
	float x4 = x * x * x * x;
	float x3 = x * x * x;
	float x2 = x * x;
	return -(x4 + 3 * x3 - 9 * x2 - 23 * x - 12) / 2;
}


std::string climb() {
	srand(time(0));
	int max = 10;
	int min = -10;
	float val = (rand() % (max - min)) + min; // rand between min and max
	float step = (max - min);
	while (true) {
		float curr = function(val);
		float up = function(val + step);
		float down = function(val - step);


		if (up > curr && up > down && val + step <= max) {
			val = val + step;
		}
		else if (down > curr && down > up && val - step >= min) {
			val = val - step;
		}
		else {
			step *= 0.99f;
			if (step < 0.01) {
				break;
			}
		}
		if (val < min) {
			val = min;
		}
		if (val > max) {
			val = max;
		}
	}


	std::cout << val << std::endl;
	return std::to_string(val);
}