#include <list>
#include <string>
#include "asema.h"
#include "nappula.h"
#include <algorithm>
using namespace std;

void Nappula::talletaSiirto(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari, int xSiirto, int ySiirto) {
	Nappula* nappula = asema->lauta[xSiirto][ySiirto];
	if (nappula == NULL || nappula->vari != vari) {
		Siirto s = Siirto(*ruutu, Ruutu(xSiirto, ySiirto));
		if (nappula != NULL) {
			s.syonti = true;
			s.syontiArvo = abs((64*nappula->arvo)) - abs(arvo);
			syonnit.push_back(s);
		}
		else {
			siirrot.push_back(s);
		}
	}
}

void Kuningas::annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari) {
	int alkuSarake = ruutu->getSarake();
	int alkuRivi = ruutu->getRivi();

	for (int x = -1; x <= 1; x++) {
		int xSiirto = x + alkuSarake;
		if (xSiirto < 8 && xSiirto >= 0) {
			for (int y = -1; y <= 1; y++) {
				int ySiirto = y + alkuRivi;
				if (ySiirto < 8 && ySiirto >= 0) {
					// Ruutu tyhjä tai eri värinen nappula, kuin siirrettävällä
					talletaSiirto(siirrot, syonnit, ruutu, asema, vari, xSiirto, ySiirto);
				}
			}
		}
	}

	if (vari == 0 && !asema->getOnkoValkeaKuningasLiikkunut()) {
		if (!asema->getOnkoValkeaKTliikkunut() && asema->lauta[5][0] == NULL && asema->lauta[6][0] == NULL) {
			siirrot.push_back(Siirto(true, false));
		}
		if (!asema->getOnkoValkeaDTliikkunut() && asema->lauta[3][0] == NULL && asema->lauta[2][0] == NULL && asema->lauta[1][0] == NULL) {
			siirrot.push_back(Siirto(false, true));
		}
	}

	if (vari == 1 && !asema->getOnkoMustaKuningasLiikkunut()) {
		if (!asema->getOnkoMustaKTliikkunut() && asema->lauta[5][7] == NULL && asema->lauta[6][7] == NULL) {
			siirrot.push_back(Siirto(true, false));
		}
		if (!asema->getOnkoMustaDTliikkunut() && asema->lauta[3][7] == NULL && asema->lauta[2][7] == NULL && asema->lauta[1][7] == NULL) {
			siirrot.push_back(Siirto(false, true));
		}
	}
}

void Sotilas::annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari) {
	int alkuSarake = ruutu->getSarake();
	int alkuRivi = ruutu->getRivi();

	//Valkoinen rivillä 2 
	if (alkuRivi == 1 && vari == 0) {
		for (int i = alkuRivi + 1; i <= alkuRivi + 2; i++) {
			int ySiirto = i;
			if (asema->lauta[alkuSarake][ySiirto] == NULL) {
				Siirto s = Siirto(*ruutu, Ruutu(alkuSarake, ySiirto));
				siirrot.push_back(s);
			}
			else {
				break;
			}
		}
	} // Musta rivillä 7
	else if (alkuRivi == 6 && vari == 1) {
		for (int i = alkuRivi - 1; i >= alkuRivi - 2; i--) {
			int ySiirto = i;
			if (asema->lauta[alkuSarake][ySiirto] == NULL) {
				Siirto s = Siirto(*ruutu, Ruutu(alkuSarake, ySiirto));
				siirrot.push_back(s);
			}
			else {
				break;
			}
		}
	}
	else if (vari == 0) {
		if (asema->lauta[alkuSarake][alkuRivi + 1] == NULL) {
			if (alkuRivi < 6) {
				Siirto s = Siirto(*ruutu, Ruutu(alkuSarake, alkuRivi + 1));
				siirrot.push_back(s);
			}
			else {
				Siirto s = Siirto(*ruutu, Ruutu(alkuSarake, alkuRivi + 1));
				s.syontiArvo = 10;
				lisaaSotilaanKorotukset(&s, siirrot, syonnit, asema, vari);
			}
		}
	}
	else if (vari == 1) {

		if (asema->lauta[alkuSarake][alkuRivi - 1] == NULL) {
			if (alkuRivi > 1) {
				Siirto s = Siirto(*ruutu, Ruutu(alkuSarake, alkuRivi - 1));
				siirrot.push_back(s);
			}
			else {
				Siirto s = Siirto(*ruutu, Ruutu(alkuSarake, alkuRivi - 1));
				s.syontiArvo = 10;
				lisaaSotilaanKorotukset(&s, siirrot, syonnit, asema, vari);
			}
		}
	}

	if (vari == 0) {
		if (alkuSarake + 1 <= 7 && alkuRivi + 1 <= 7) {
			if (asema->lauta[alkuSarake + 1][alkuRivi + 1] != NULL && asema->lauta[alkuSarake + 1][alkuRivi + 1]->vari != vari) {
				if (alkuRivi < 6) {
					Siirto s = Siirto(*ruutu, Ruutu(alkuSarake + 1, alkuRivi + 1));
					s.syontiArvo = abs(64 * asema->lauta[alkuSarake + 1][alkuRivi + 1]->arvo) - abs(arvo);
					s.syonti = true;
					syonnit.push_back(s);
				}
				else {

					Siirto s = Siirto(*ruutu, Ruutu(alkuSarake + 1, alkuRivi + 1));
					s.syontiArvo = abs(64 * asema->lauta[alkuSarake + 1][alkuRivi + 1]->arvo) + 10;
					s.syonti = true;
					lisaaSotilaanKorotukset(&s, siirrot, syonnit, asema, vari);
				}
			}
		}
		if (alkuSarake - 1 >= 0 && alkuRivi + 1 <= 7) {
			if (asema->lauta[alkuSarake - 1][alkuRivi + 1] != NULL && asema->lauta[alkuSarake - 1][alkuRivi + 1]->vari != vari) {
				if (alkuRivi < 6) {
					Siirto s = Siirto(*ruutu, Ruutu(alkuSarake - 1, alkuRivi + 1));
					s.syontiArvo = abs(64 * asema->lauta[alkuSarake - 1][alkuRivi + 1]->arvo) - abs(arvo);
					s.syonti = true;
					syonnit.push_back(s);
				}
				else {
					Siirto s = Siirto(*ruutu, Ruutu(alkuSarake - 1, alkuRivi + 1));
					s.syontiArvo = abs(64 * asema->lauta[alkuSarake - 1][alkuRivi + 1]->arvo) + 10;
					s.syonti = true;
					lisaaSotilaanKorotukset(&s, siirrot, syonnit, asema, vari);
				}
			}
		}

		//En passant
		if (asema->kaksoisaskelSarakkeella != -1 && alkuRivi == 4) {
			if (alkuSarake > 0 && asema->kaksoisaskelSarakkeella == alkuSarake - 1) {
				//en passanat vasemmalle
				Siirto s(*ruutu, Ruutu(alkuSarake - 1, alkuRivi + 1));
				s.setEnPassant(true);
				s.syontiArvo = 64;
				s.syonti = true;
				syonnit.push_back(s);
			}
			else if (alkuSarake < 7 && asema->kaksoisaskelSarakkeella == alkuSarake + 1) {
				//en passanat oikealle
				Siirto s(*ruutu, Ruutu(alkuSarake + 1, alkuRivi + 1));
				s.setEnPassant(true);
				s.syontiArvo = 64;
				s.syonti = true;
				syonnit.push_back(s);
			}
		}
	}
	else if (vari == 1) {
		if (alkuSarake + 1 <= 7 && alkuRivi - 1 >= 0) {
			if (asema->lauta[alkuSarake + 1][alkuRivi - 1] != NULL && asema->lauta[alkuSarake + 1][alkuRivi - 1]->vari != vari) {
				if (alkuRivi > 1) {
					Siirto s = Siirto(*ruutu, Ruutu(alkuSarake + 1, alkuRivi - 1));
					s.syontiArvo = abs(64 * asema->lauta[alkuSarake + 1][alkuRivi - 1]->arvo) - abs(arvo);
					syonnit.push_back(s);
				}
				else {
					Siirto s = Siirto(*ruutu, Ruutu(alkuSarake + 1, alkuRivi - 1));
					s.syontiArvo = abs(64 * asema->lauta[alkuSarake + 1][alkuRivi - 1]->arvo) + 10;
					lisaaSotilaanKorotukset(&s, siirrot, syonnit, asema, vari);
				}
			}
		}
		if (alkuSarake - 1 >= 0 && alkuRivi - 1 >= 0) {
			if (asema->lauta[alkuSarake - 1][alkuRivi - 1] != NULL && asema->lauta[alkuSarake - 1][alkuRivi - 1]->vari != vari) {
				if (alkuRivi > 1) {
					Siirto s = Siirto(*ruutu, Ruutu(alkuSarake - 1, alkuRivi - 1));
					s.syontiArvo = abs(64 * asema->lauta[alkuSarake - 1][alkuRivi - 1]->arvo) - abs(arvo);
					s.syonti = true;
					syonnit.push_back(s);
				}
				else {
					Siirto s = Siirto(*ruutu, Ruutu(alkuSarake - 1, alkuRivi - 1));
					s.syontiArvo = abs(64 * asema->lauta[alkuSarake - 1][alkuRivi - 1]->arvo) + 10;
					s.syonti = true;
					lisaaSotilaanKorotukset(&s, siirrot, syonnit, asema, vari);
				}
			}
		}

		//En passant
		if (asema->kaksoisaskelSarakkeella != -1 && alkuRivi == 3) {
			if (alkuSarake > 0 && asema->kaksoisaskelSarakkeella == alkuSarake - 1) {
				//en passanat vasemmalle
				Siirto s(*ruutu, Ruutu(alkuSarake - 1, alkuRivi - 1));
				s.setEnPassant(true);
				s.syontiArvo = 63;
				s.syonti = true;
				syonnit.push_back(s);
			}
			else if (alkuSarake < 7 && asema->kaksoisaskelSarakkeella == alkuSarake + 1) {
				//en passanat oikealle
				Siirto s(*ruutu, Ruutu(alkuSarake + 1, alkuRivi - 1));
				s.setEnPassant(true);
				s.syontiArvo = 63;
				s.syonti = true;
				syonnit.push_back(s);
			}
		}
	}


}

void Sotilas::lisaaSotilaanKorotukset(Siirto* siirto, std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Asema* asema, int vari) {
	if (siirto->syonti) {
		if (vari == 0) {
			siirto->setMiksiKorotetaan(VD);
			syonnit.push_back(*siirto);
			siirto->setMiksiKorotetaan(VT);
			syonnit.push_back(*siirto);
			siirto->setMiksiKorotetaan(VL);
			syonnit.push_back(*siirto);
			siirto->setMiksiKorotetaan(VR);
			syonnit.push_back(*siirto);
		}
		else if (vari == 1) {
			siirto->setMiksiKorotetaan(MD);
			syonnit.push_back(*siirto);
			siirto->setMiksiKorotetaan(MT);
			syonnit.push_back(*siirto);
			siirto->setMiksiKorotetaan(ML);
			syonnit.push_back(*siirto);
			siirto->setMiksiKorotetaan(MR);
			syonnit.push_back(*siirto);
		}
	}
	else {
		if (vari == 0) {
			siirto->setMiksiKorotetaan(VD);
			siirrot.push_back(*siirto);
			siirto->setMiksiKorotetaan(VT);
			siirrot.push_back(*siirto);
			siirto->setMiksiKorotetaan(VL);
			siirrot.push_back(*siirto);
			siirto->setMiksiKorotetaan(VR);
			siirrot.push_back(*siirto);
		}
		else if (vari == 1) {
			siirto->setMiksiKorotetaan(MD);
			siirrot.push_back(*siirto);
			siirto->setMiksiKorotetaan(MT);
			siirrot.push_back(*siirto);
			siirto->setMiksiKorotetaan(ML);
			siirrot.push_back(*siirto);
			siirto->setMiksiKorotetaan(MR);
			siirrot.push_back(*siirto);
		}
	}
	
}

void Daami::annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari) {
	int maara = siirrot.size();
	this->Lahetti::annaSiirrot(siirrot, syonnit, ruutu, asema, vari);
	this->Torni::annaSiirrot(siirrot, syonnit, ruutu, asema, vari);
	maara = siirrot.size() - maara;

	vari == 0 ? asema->vDsiirrot = maara : asema->mDsiirrot = maara;
}

void Nappula::liikuRatsu(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari, int xSuunta, int ySuunta) {
	int alkuSarake = ruutu->getSarake();
	int alkuRivi = ruutu->getRivi();

	int xSiirto = alkuSarake + xSuunta;
	int ySiirto = alkuRivi + ySuunta;

	if (xSiirto > 7 || ySiirto > 7 || xSiirto < 0 || ySiirto < 0) {
		return;
	}

	talletaSiirto(siirrot, syonnit, ruutu, asema, vari, xSiirto, ySiirto);
}

void Nappula::liikuSuorassa(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari, int xSuunta, int ySuunta) {
	int alkuSarake = ruutu->getSarake();
	int alkuRivi = ruutu->getRivi();

	int xSiirto = alkuSarake;
	int ySiirto = alkuRivi;

	for (int i = 0; i < 8; i++) {
		xSiirto += xSuunta;
		ySiirto += ySuunta;

		if (xSiirto > 7 || ySiirto > 7 || xSiirto < 0 || ySiirto < 0) {
			break;
		}

		if (asema->lauta[xSiirto][ySiirto] != NULL && asema->lauta[xSiirto][ySiirto]->vari == vari) {
			break;
		}

		if (asema->lauta[xSiirto][ySiirto] == NULL) {
			talletaSiirto(siirrot, syonnit, ruutu, asema, vari, xSiirto, ySiirto);
		}
		else if (asema->lauta[xSiirto][ySiirto]->vari != vari) {
			talletaSiirto(siirrot, syonnit, ruutu, asema, vari, xSiirto, ySiirto);
			break;
		}
	}
}

void Lahetti::annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari) {
	int maara = siirrot.size();
	//Oikealle ylös
	liikuSuorassa(siirrot, syonnit, ruutu, asema, vari, 1, 1);
	//Oikealle alas
	liikuSuorassa(siirrot, syonnit, ruutu, asema, vari, 1, -1);
	//Vasemmalle ylös
	liikuSuorassa(siirrot, syonnit, ruutu, asema, vari, -1, 1);
	//Vasemmalle alas
	liikuSuorassa(siirrot, syonnit, ruutu, asema, vari, -1, -1);

	maara = siirrot.size() - maara;

	vari == 0 ? asema->vLsiirrot = maara : asema->mLsiirrot = maara;
}

void Ratsu::annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari) {
	liikuRatsu(siirrot, syonnit, ruutu, asema, vari, -1, 2);
	liikuRatsu(siirrot, syonnit, ruutu, asema, vari, 1, 2);
	liikuRatsu(siirrot, syonnit, ruutu, asema, vari, 2, 1);
	liikuRatsu(siirrot, syonnit, ruutu, asema, vari, 2, -1);
	liikuRatsu(siirrot, syonnit, ruutu, asema, vari, -1, -2);
	liikuRatsu(siirrot, syonnit, ruutu, asema, vari, 1, -2);
	liikuRatsu(siirrot, syonnit, ruutu, asema, vari, -2, 1);
	liikuRatsu(siirrot, syonnit, ruutu, asema, vari, -2, -1);
}

void Torni::annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari) {
	int maara = siirrot.size();
	//Oikealle
	liikuSuorassa(siirrot, syonnit, ruutu, asema, vari, 1, 0);
	//Ylös
	liikuSuorassa(siirrot, syonnit, ruutu, asema, vari, 0, 1);
	//Vasemmalle
	liikuSuorassa(siirrot, syonnit, ruutu, asema, vari, -1, 0);
	//Alas
	liikuSuorassa(siirrot, syonnit, ruutu, asema, vari, 0, -1);

	maara = siirrot.size() - maara;

	vari == 0 ? asema->vTsiirrot = maara : asema->mTsiirrot = maara;
}
