#include "Ruutu.h"

Ruutu::Ruutu(int sarake, int rivi) {
	this->sarake = sarake;
	this->rivi = rivi;
}

Ruutu::Ruutu() {
	sarake = 0;
	rivi = 0;
}

int Ruutu::getRivi() const {
	return rivi;
}

int Ruutu::getSarake() const {
	return sarake;
}

bool Ruutu::operator==(const Ruutu& other) const {
	return sarake == other.sarake && rivi == other.rivi;
}
