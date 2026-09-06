#include "hashing.h"
#include "asema.h"
#include <iostream>
#include <random>
#include <limits>

long random() {
	static std::random_device rd;
	static std::mt19937_64 eng(rd());

	static std::uniform_int_distribution<long> distr;

	return distr(eng);
}

long Hash::nappulat[12][8][8];
long Hash::mustanVuoro;

long Hash::onkoValkeaKuningasLiikkunutHash;
long Hash::onkoMustaKuningasLiikkunutHash;
long Hash::onkoValkeaDTliikkunutHash;
long Hash::onkoValkeaKTliikkunutHash;
long Hash::onkoMustaDTliikkunutHash;
long Hash::onkoMustaKTliikkunutHash;

long Hash::kaksoisaskel[8];

void Hash::init() {
	//luo random numerot jokaiselle static longille
	for (size_t i = 0; i < 12; i++) {
		for (size_t j = 0; j < 8; j++) {
			for (size_t k = 0; k < 8; k++) {
				nappulat[i][j][k] = random();
			}
		}
	}
	mustanVuoro = random();
	onkoValkeaKuningasLiikkunutHash = random();
	onkoMustaKuningasLiikkunutHash = random();
	onkoValkeaDTliikkunutHash = random();
	onkoValkeaKTliikkunutHash = random();
	onkoMustaDTliikkunutHash = random();
	onkoMustaKTliikkunutHash = random();

	for (size_t k = 0; k < 8; k++) {
		kaksoisaskel[k] = random();
	}
}

void Hash::rebuild(Asema* asema) {

	//Initialisoi hash 0
	hash = 0;

	//looppaa laudan läpi
	for (size_t j = 0; j < 8; j++) {
		for (size_t k = 0; k < 8; k++) {
			if (asema->lauta[j][k] != NULL) {
				//hae jokaisen nappulan kohdalla listasta numero
				int i = asema->lauta[j][k]->_koodi;
				//hash = hash xor nappulanumero
				hash = hash ^ nappulat[i][j][k];
			}
		}
	}
	//hash xor mustanvuoro jos musta
	if (asema->getSiirtovuoro() == 1) {
		hash = hash ^ mustanVuoro;
	}
	//hash xor onkoValkeaKuningasLiikkunut jos liikkunut
	if (asema->getOnkoValkeaKuningasLiikkunut()) {
		hash = hash ^ onkoValkeaKuningasLiikkunutHash;
	}
	//hash xor onkoMustaKuningasLiikkunut jos liikkunut
	if (asema->getOnkoMustaKuningasLiikkunut()) {
		hash = hash ^ onkoMustaKuningasLiikkunutHash;
	}
	//hash xor onkoValkeaDTliikkunut jos liikkunut
	if (asema->getOnkoValkeaDTliikkunut()) {
		hash = hash ^ onkoValkeaDTliikkunutHash;
	}
	//hash xor onkoValkeaKTliikkunut jos liikkunut
	if (asema->getOnkoValkeaKTliikkunut()) {
		hash = hash ^ onkoValkeaKTliikkunutHash;
	}
	//hash xor onkoMustaDTliikkunut jos liikkunut
	if (asema->getOnkoMustaDTliikkunut()) {
		hash = hash ^ onkoMustaDTliikkunutHash;
	}
	//hash xor onkoMustaKTliikkunut jos liikkunut
	if (asema->getOnkoMustaKTliikkunut()) {
		hash = hash ^ onkoMustaKTliikkunutHash;
	}
	//hash xor kaksoisaskel jos ei -1
	if (asema->kaksoisaskelSarakkeella != -1) {
		hash = hash ^ kaksoisaskel[asema->kaksoisaskelSarakkeella];
	}
}


void Hash::update(Siirto* siirto, Asema* asema) {

	//update siirto to hash
	auto lauta = asema->lauta;
	int alkuX = siirto->getAlkuruutu().getSarake();
	int alkuY = siirto->getAlkuruutu().getRivi();
	int loppuX = siirto->getLoppuruutu().getSarake();
	int loppuY = siirto->getLoppuruutu().getRivi();
	Nappula* nappula = lauta[alkuX][alkuY];
	Nappula* syoty = lauta[loppuX][loppuY];

	if (!siirto->onkoLyhytLinna() && !siirto->onkoPitkalinna()) {
		int koodi = nappula->_koodi;

		hash = hash ^ nappulat[koodi][alkuX][alkuY];
		hash = hash ^ nappulat[koodi][loppuX][loppuY];

		if (syoty != NULL) {
			int syotyKoodi = syoty->_koodi;
			hash = hash ^ nappulat[syotyKoodi][loppuX][loppuY];
		}

		if (!asema->getOnkoMustaKuningasLiikkunut() &&
			((alkuX == 4 && alkuY == 7) ||
			(loppuX == 4 && loppuY == 7))) {
			hash = hash ^ onkoMustaKuningasLiikkunutHash;
		}
		if (!asema->getOnkoMustaDTliikkunut() &&
			((alkuX == 0 && alkuY == 7) ||
			(loppuX == 0 && loppuY == 7))) {
			hash = hash ^ onkoMustaDTliikkunutHash;
		}
		if (!asema->getOnkoMustaKTliikkunut() &&
			((alkuX == 7 && alkuY == 7) ||
			(loppuX == 7 && loppuY == 7))) {
			hash = hash ^ onkoMustaKTliikkunutHash;
		}

		if (!asema->getOnkoValkeaKuningasLiikkunut() && 
			((alkuX == 4 && alkuY == 0) ||
			(loppuX == 4 && loppuY == 0))) {
			hash = hash ^ onkoValkeaKuningasLiikkunutHash;
		}
		if (!asema->getOnkoValkeaDTliikkunut() &&
			((alkuX == 0 && alkuY == 0) ||
			(loppuX == 0 && loppuY == 0))) {
			hash = hash ^ onkoValkeaDTliikkunutHash;
		}
		if (!asema->getOnkoValkeaKTliikkunut() &&
			((alkuX == 7 && alkuY == 0) ||
			(loppuX == 7 && loppuY == 0))) {
			hash = hash ^ onkoValkeaKTliikkunutHash;
		}
	}
	else if (siirto->onkoLyhytLinna()) {
		if (asema->getSiirtovuoro() == 0) { //valkoinen
			hash = hash ^ nappulat[VK][4][0];
			hash = hash ^ nappulat[VT][7][0];
			hash = hash ^ nappulat[VK][6][0];
			hash = hash ^ nappulat[VT][5][0];

			hash = hash ^ onkoValkeaKuningasLiikkunutHash;
			hash = hash ^ onkoValkeaKTliikkunutHash;
		}
		else { //musta
			hash = hash ^ nappulat[MK][4][7];
			hash = hash ^ nappulat[MT][7][7];
			hash = hash ^ nappulat[MK][6][7];
			hash = hash ^ nappulat[MT][5][7];


			hash = hash ^ onkoMustaKuningasLiikkunutHash;
			hash = hash ^ onkoMustaKTliikkunutHash;
		}
	}
	else if (siirto->onkoPitkalinna()) {
		if (asema->getSiirtovuoro() == 0) { //valkoinen
			hash = hash ^ nappulat[VK][4][0];
			hash = hash ^ nappulat[VT][0][0];
			hash = hash ^ nappulat[VK][2][0];
			hash = hash ^ nappulat[VT][3][0];

			hash = hash ^ onkoValkeaKuningasLiikkunutHash;
			hash = hash ^ onkoValkeaDTliikkunutHash;
		}
		else{ //musta
			hash = hash ^ nappulat[MK][4][7];
			hash = hash ^ nappulat[MT][0][7];
			hash = hash ^ nappulat[MK][2][7];
			hash = hash ^ nappulat[MT][3][7];

			hash = hash ^ onkoMustaKuningasLiikkunutHash;
			hash = hash ^ onkoMustaDTliikkunutHash;
		}
	}

	// Kaksoisaskel

	if (nappula != NULL && (nappula == Asema::vs || nappula == Asema::ms) &&
		abs(loppuY - alkuY) == 2) {
		hash = hash ^ kaksoisaskel[loppuX];
	}
	if(asema->kaksoisaskelSarakkeella > -1){
		hash = hash ^ kaksoisaskel[asema->kaksoisaskelSarakkeella];
	}

	if (nappula != NULL && (nappula == Asema::vs || nappula == Asema::ms) && (loppuY == 0 ||
		loppuY == 7)) {
		int koodi = nappula->_koodi;
		hash = hash ^ nappulat[koodi][loppuX][loppuY];
		hash = hash ^ nappulat[siirto->getMiksiKorotetaan()][loppuX][loppuY];
	}

	// En passant
	if (siirto->onkoEnPassant()) {
		int syotyKoodi = lauta[loppuX][alkuY]->_koodi;
		hash = hash ^ nappulat[syotyKoodi][loppuX][alkuY];
	}

	hash = hash ^ mustanVuoro;
}

bool Hash::tarkista(Asema* asema) {
#if _DEBUG
	Hash tarkistus = Hash(*this);
	tarkistus.rebuild(asema);
	if (tarkistus.hash != hash) {
		std::cout << std::endl;
		std::cout << "ERROR HASH " << std::endl;
		
		return false;
	}
#endif
	return true;
}
