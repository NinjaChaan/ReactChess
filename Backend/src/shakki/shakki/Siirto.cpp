#include "Siirto.h"
#include "asema.h"
#include <sstream>
Siirto::Siirto(Ruutu alku, Ruutu loppu) {
	alkuRuutu = alku;
	loppuRuutu = loppu;
	miksiKorotetaan = 0;
	lyhytLinna = false;
	pitkalinna = false;
	enPassant = false;
	syonti = false;
}

Siirto::Siirto() {
	alkuRuutu = Ruutu();
	loppuRuutu = Ruutu();
	miksiKorotetaan = 0;
	lyhytLinna = false;
	pitkalinna = false;
	enPassant = false;
	syonti = false;
}

Siirto::Siirto(bool lyhytLinna, bool pitkaLinna) {
	alkuRuutu = Ruutu();
	loppuRuutu = Ruutu();
	miksiKorotetaan = 0;
	this->lyhytLinna = lyhytLinna;
	this->pitkalinna = pitkaLinna;
	enPassant = false;
	syonti = false;
}

Ruutu Siirto::getAlkuruutu() {
	return alkuRuutu;
}

Ruutu Siirto::getLoppuruutu() {
	return loppuRuutu;
}

bool Siirto::onkoLyhytLinna() {
	return lyhytLinna;
}

bool Siirto::onkoPitkalinna() {
	return pitkalinna;
}

bool Siirto::onkoEnPassant() {
	return enPassant;
}

int Siirto::getMiksiKorotetaan() {
	return miksiKorotetaan;
}

void Siirto::setMiksiKorotetaan(int miksiKorotetaan) {
	this->miksiKorotetaan = miksiKorotetaan;
}

void Siirto::setEnPassant(bool enPassant) {
	this->enPassant = enPassant;
}

std::wstring Siirto::toString(Asema* asema) {
	Nappula* nappula = asema->lauta[alkuRuutu.getSarake()][alkuRuutu.getRivi()];

	std::wstringstream ss;

	if (lyhytLinna) {
		return L"O-O";
	}
	else if (pitkalinna) {
		return L"O-O-O";
	}

	if (nappula == Asema::vk || nappula == Asema::mk) {
		ss << " K";
	}
	else if (nappula == Asema::vd || nappula == Asema::md) {
		ss << " D";
	}
	else if (nappula == Asema::vl || nappula == Asema::ml) {
		ss << " L";
	}
	else if (nappula == Asema::vt || nappula == Asema::mt) {
		ss << " T";
	}
	else if (nappula == Asema::vr || nappula == Asema::mr) {
		ss << " R";
	}
	else if (nappula == Asema::vs || nappula == Asema::ms) {
		if (!(getLoppuruutu().getRivi() == 0 || getLoppuruutu().getRivi() == 7)) {
			ss << "  ";
		}
	}

	ss << (char)(alkuRuutu.getSarake() + 'a');
	ss << (char)(alkuRuutu.getRivi() + '1');
	ss << '-';
	ss << (char)(loppuRuutu.getSarake() + 'a');
	ss << (char)(loppuRuutu.getRivi() + '1');


	if (nappula != NULL && (nappula == Asema::vs || nappula == Asema::ms) && (getLoppuruutu().getRivi() == 0 ||
		getLoppuruutu().getRivi() == 7)) {

		switch (getMiksiKorotetaan()) {
		case VD: case MD:
			ss << L"=D";
			break;
		case VL: case ML:
			ss << L"=L";
			break;
		case VT: case MT:
			ss << L"=T";
			break;
		case VR: case MR:
			ss << L"=R";
			break;
		}
	}

	return ss.str();
}

bool Siirto::operator==(const Siirto& other) const {
	if (lyhytLinna == true && other.lyhytLinna == true) return true;
	if (pitkalinna == true && other.pitkalinna == true) return true;
	if (!lyhytLinna && !pitkalinna && !other.lyhytLinna && !other.pitkalinna && alkuRuutu.getRivi() == other.alkuRuutu.getRivi() &&
		alkuRuutu.getSarake() == other.alkuRuutu.getSarake() &&
		loppuRuutu.getRivi() == other.loppuRuutu.getRivi() &&
		loppuRuutu.getSarake() == other.loppuRuutu.getSarake() &&
		miksiKorotetaan == other.miksiKorotetaan) {
		return true;
	}
	return false;
}

bool Siirto::operator!=(const Siirto& other) const {
	return !(*this == other);
}

bool Siirto::operator<(const Siirto& other)const {
	return this->syontiArvo < other.syontiArvo;
}

bool Siirto::operator>(const Siirto& other)const {
	return this->syontiArvo > other.syontiArvo;
}
