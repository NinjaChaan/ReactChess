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
	} else if (pitkalinna) {
		return L"O-O-O";
	}

	if (nappula == Asema::vk || nappula == Asema::mk) {
		ss << " K";
	} else if (nappula == Asema::vd || nappula == Asema::md) {
		ss << " D";
	} else if (nappula == Asema::vl || nappula == Asema::ml) {
		ss << " L";
	} else if (nappula == Asema::vt || nappula == Asema::mt) {
		ss << " T";
	} else if (nappula == Asema::vr || nappula == Asema::mr) {
		ss << " R";
	} else if (nappula == Asema::vs || nappula == Asema::ms) {
		if (!(getLoppuruutu().getRivi() == 0 || getLoppuruutu().getRivi() == 7)) {
			ss << "  ";
		}
	}

	ss << (char)(alkuRuutu.getSarake() + 'a');
	ss << (char)(alkuRuutu.getRivi() + '1');
	ss << '-';
	ss << (char)(loppuRuutu.getSarake() + 'a');
	ss << (char)(loppuRuutu.getRivi() + '1');

	if (nappula != NULL && (nappula == Asema::vs || nappula == Asema::ms) && (getLoppuruutu().getRivi() == 0 || getLoppuruutu().getRivi() == 7)) {

		switch (getMiksiKorotetaan()) {
		case VD:
		case MD:
			ss << L"=D";
			break;
		case VL:
		case ML:
			ss << L"=L";
			break;
		case VT:
		case MT:
			ss << L"=T";
			break;
		case VR:
		case MR:
			ss << L"=R";
			break;
		}
	}

	return ss.str();
}

std::string Siirto::toNormalString(Asema* asema) {
	Nappula* nappula = asema->lauta[alkuRuutu.getSarake()][alkuRuutu.getRivi()];
	Nappula* vnappula = asema->lauta[loppuRuutu.getSarake()][loppuRuutu.getRivi()];

	std::stringstream ss;

	if (lyhytLinna) {
		return "O-O";
	} else if (pitkalinna) {
		return "O-O-O";
	} else if (enPassant) {
		ss << (char)(alkuRuutu.getSarake() + 'a');
		ss << (char)(alkuRuutu.getRivi() + '1');
		ss << 'x';
		ss << (char)(loppuRuutu.getSarake() + 'a');
		ss << (char)(loppuRuutu.getRivi() + '1');
		return ss.str();
	}

	if (nappula == Asema::vk || nappula == Asema::mk) {
		ss << "K";
	} else if (nappula == Asema::vd || nappula == Asema::md) {
		ss << "Q";
	} else if (nappula == Asema::vl || nappula == Asema::ml) {
		ss << "B";
	} else if (nappula == Asema::vt || nappula == Asema::mt) {
		ss << "R";
	} else if (nappula == Asema::vr || nappula == Asema::mr) {
		ss << "N";
	} else if (nappula == Asema::vs || nappula == Asema::ms) {
		if (!(getLoppuruutu().getRivi() == 0 || getLoppuruutu().getRivi() == 7)) {
			ss << "";
		}
	}

	ss << (char)(alkuRuutu.getSarake() + 'a');
	ss << (char)(alkuRuutu.getRivi() + '1');
	if (vnappula != nullptr) {
		ss << 'x';
	} else {
		ss << '-';
	}
	ss << (char)(loppuRuutu.getSarake() + 'a');
	ss << (char)(loppuRuutu.getRivi() + '1');

	if (nappula != NULL && (nappula == Asema::vs || nappula == Asema::ms) && (getLoppuruutu().getRivi() == 0 || getLoppuruutu().getRivi() == 7)) {
		switch (getMiksiKorotetaan()) {
		case VD:
		case MD:
			ss << "=Q";
			break;
		case VL:
		case ML:
			ss << "=B";
			break;
		case VT:
		case MT:
			ss << "=R";
			break;
		case VR:
		case MR:
			ss << "=N";
			break;
		}
	}
	Asema uusiAsema = Asema(*asema);
	uusiAsema.paivitaAsema(this);
	if (uusiAsema.onkoKuningasUhattu()) {
		ss << "+";
	}

	return ss.str();
}

std::string Siirto::toUCIString(Asema* asema) {
	Nappula* nappula = asema->lauta[alkuRuutu.getSarake()][alkuRuutu.getRivi()];

	std::stringstream ss;

	if (lyhytLinna) {
		return asema->getSiirtovuoro() == 0 ? "e1g1" : "e8g8";
	} else if (pitkalinna) {
		return asema->getSiirtovuoro() == 0 ? "e1c1" : "e8c8";
	}

	ss << (char)(alkuRuutu.getSarake() + 'a');
	ss << (char)(alkuRuutu.getRivi() + '1');
	ss << (char)(loppuRuutu.getSarake() + 'a');
	ss << (char)(loppuRuutu.getRivi() + '1');

	if (nappula != NULL && (nappula == Asema::vs || nappula == Asema::ms) && (getLoppuruutu().getRivi() == 0 || getLoppuruutu().getRivi() == 7)) {
		switch (getMiksiKorotetaan()) {
		case VD:
		case MD:
			ss << "q";
			break;
		case VL:
		case ML:
			ss << "b";
			break;
		case VT:
		case MT:
			ss << "r";
			break;
		case VR:
		case MR:
			ss << "n";
			break;
		}
	}

	return ss.str();
}

Siirto Siirto::fromUCIString(std::string uciMove, Asema* asema) {

	if (uciMove.size() == 5) {
		char r1c = uciMove.c_str()[0] - 'a';
		char r1r = uciMove.c_str()[1] - '1';
		char r2c = uciMove.c_str()[2] - 'a';
		char r2r = uciMove.c_str()[3] - '1';

		Ruutu ruutu1(r1c, r1r);
		Ruutu ruutu2(r2c, r2r);

		int vari = asema->getNappulaRuudussa(ruutu1.getSarake(), ruutu1.getRivi())->getVari();

		Siirto siirto = Siirto(ruutu1, ruutu2);

		char promo = uciMove.c_str()[4];

		switch (promo) {
		case 'q':
			if (vari == 0) {
				siirto.setMiksiKorotetaan(VD);
			} else {
				siirto.setMiksiKorotetaan(MD);
			}
			break;
		case 'r':
			if (vari == 0) {
				siirto.setMiksiKorotetaan(VT);
			} else {
				siirto.setMiksiKorotetaan(MT);
			}
			break;
		case 'b':
			if (vari == 0) {
				siirto.setMiksiKorotetaan(VL);
			} else {
				siirto.setMiksiKorotetaan(ML);
			}
			break;
		case 'n':
			if (vari == 0) {
				siirto.setMiksiKorotetaan(VR);
			} else {
				siirto.setMiksiKorotetaan(MR);
			}
			break;
		}

		//promotion
	} else if (uciMove.size() == 4) {
		char r1c = uciMove.c_str()[0] - 'a';
		char r1r = uciMove.c_str()[1] - '1';
		char r2c = uciMove.c_str()[2] - 'a';
		char r2r = uciMove.c_str()[3] - '1';

		Ruutu ruutu1(r1c, r1r);
		Ruutu ruutu2(r2c, r2r);

		Nappula* nappula = asema->getNappulaRuudussa(ruutu1.getSarake(), ruutu1.getRivi());
		Nappula* eatsNappula = asema->getNappulaRuudussa(ruutu2.getSarake(), ruutu2.getRivi());

		Siirto siirto(ruutu1, ruutu2);

		if ((nappula->getKoodi() == VS || nappula->getKoodi() == MS) && r1c != r2c && eatsNappula == nullptr) {
			//en passant happened!
			siirto.setEnPassant(true);
		}

		if (nappula->getKoodi() == VK) {
			if (ruutu1.getRivi() == 0 && ruutu1.getSarake() == 4 && ruutu2.getRivi() == 0 && ruutu2.getSarake() == 6) {
				return Siirto(true, false);
			} else if (ruutu1.getRivi() == 0 && ruutu1.getSarake() == 4 && ruutu2.getRivi() == 0 && ruutu2.getSarake() == 2) {
				return Siirto(false, true);
			}
		} else if (nappula->getKoodi() == MK) {
			if (ruutu1.getRivi() == 7 && ruutu1.getSarake() == 4 && ruutu2.getRivi() == 7 && ruutu2.getSarake() == 6) {
				return Siirto(true, false);
			} else if (ruutu1.getRivi() == 7 && ruutu1.getSarake() == 4 && ruutu2.getRivi() == 7 && ruutu2.getSarake() == 2) {
				return Siirto(false, true);
			}
		}

		return siirto;
	} else {
		return Siirto();
	}
}

bool Siirto::operator==(const Siirto& other) const {
	if (lyhytLinna == true && other.lyhytLinna == true)
		return true;
	if (pitkalinna == true && other.pitkalinna == true)
		return true;
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

bool Siirto::operator<(const Siirto& other) const {
	return this->syontiArvo < other.syontiArvo;
}

bool Siirto::operator>(const Siirto& other) const {
	return this->syontiArvo > other.syontiArvo;
}
