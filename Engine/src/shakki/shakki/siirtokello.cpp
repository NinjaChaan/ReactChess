#include "siirtokello.h"


void Siirtokello::Aloita(float seconds) {
	startTime = clock::now();
	endTime = (sec)seconds;
}

void Siirtokello::Lopeta() {
	endTime = (sec)0;
}

bool Siirtokello::AikaLoppu() {
	const sec duration = clock::now() - startTime;
	return duration >= endTime;
}
