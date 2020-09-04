#pragma once
#include <chrono>



class Siirtokello {
	using clock = std::chrono::system_clock;
	using sec = std::chrono::duration<double>;
private:
	std::chrono::time_point<clock> startTime;
	sec endTime;
public:
	void Aloita(int sec);
	bool AikaLoppu();
};