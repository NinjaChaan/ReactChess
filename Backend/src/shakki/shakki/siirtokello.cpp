#include "siirtokello.h"


void Siirtokello::Aloita(int seconds) {
	startTime = clock::now();
	endTime = (sec)seconds;
}

bool Siirtokello::AikaLoppu() {
	const sec duration = clock::now() - startTime;
	return duration >= endTime;
}
