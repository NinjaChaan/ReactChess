#include <Windows.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include "kayttoliittyma.h"

using namespace std;

bool checkInRange07(char c) {
	if (c < 0 || c > 7) {
		return false;
	}
	return true;
}

Siirto Kayttoliittyma::annaVastustajanSiirto() {
	while (true) {
		wcout << endl << "Anna siirto: ";
		string siirtoStr;
		cin >> siirtoStr;

		if (siirtoStr == "O-O") {
			return Siirto(true, false);
		}
		else if (siirtoStr == "O-O-O") {
			return Siirto(false, true);
		}
		else if (siirtoStr.size() == 6 && siirtoStr.c_str()[3] == '-') {
			switch (siirtoStr.c_str()[0]) {
			case('T'): case('L'): case('R'): case('D'): case('K'):
				break;
			default:
				continue;
			}
			char r1c = siirtoStr.c_str()[1] - 'a';
			char r1r = siirtoStr.c_str()[2] - '1';
			char r2c = siirtoStr.c_str()[4] - 'a';
			char r2r = siirtoStr.c_str()[5] - '1';

			if (!checkInRange07(r1c) || !checkInRange07(r1r) ||
				!checkInRange07(r2c) || !checkInRange07(r2r)) {
				continue;
			}

			Ruutu ruutu1(r1c, r1r);
			Ruutu ruutu2(r2c, r2r);
			return Siirto(ruutu1, ruutu2);
		}
		else if (siirtoStr.size() == 5 && siirtoStr.c_str()[2] == '-') {
			char r1c = siirtoStr.c_str()[0] - 'a';
			char r1r = siirtoStr.c_str()[1] - '1';
			char r2c = siirtoStr.c_str()[3] - 'a';
			char r2r = siirtoStr.c_str()[4] - '1';

			if (!checkInRange07(r1c) || !checkInRange07(r1r) ||
				!checkInRange07(r2c) || !checkInRange07(r2r)) {
				continue;
			}
			Ruutu ruutu1(r1c, r1r);
			Ruutu ruutu2(r2c, r2r);
			Siirto siirto = Siirto(ruutu1, ruutu2);

			//Sotilaan korotus
			if (r2r == 0 || r2r == 7) {
				if (asema->getNappulaRuudussa(r1c, r1r)->_koodi == VS ||
					asema->getNappulaRuudussa(r1c, r1r)->_koodi == MS) {
					wcout << endl << "Sotilaan korotus? D, T, L, R: ";
					string korotusStr;
					cin >> korotusStr;
					if (korotusStr.length() != 1) {
						continue;
					}
					int vari = r2r == 0 ? 1 : 0;

					switch (korotusStr.c_str()[0]) {
					case 'D':
						if (vari == 0) {
							siirto.setMiksiKorotetaan(VD);
						}
						else {
							siirto.setMiksiKorotetaan(MD);
						}
						break;
					case 'T':
						if (vari == 0) {
							siirto.setMiksiKorotetaan(VT);
						}
						else {
							siirto.setMiksiKorotetaan(MT);
						}
						break;
					case 'L':
						if (vari == 0) {
							siirto.setMiksiKorotetaan(VL);
						}
						else {
							siirto.setMiksiKorotetaan(ML);
						}
						break;
					case 'R':
						if (vari == 0) {
							siirto.setMiksiKorotetaan(VR);
						}
						else {
							siirto.setMiksiKorotetaan(MR);
						}
						break;
					}
				}
			}

			return siirto;
		}
	}
}

void Kayttoliittyma::piirraLauta(Siirto* viimeSiirto) {
	//Saa unicode shakkinappulat toimimaan cout:n kanssa:
	_setmode(_fileno(stdout), _O_U16TEXT);
	int alkuSarake, alkuRivi, loppuSarake, loppuRivi;
	if (viimeSiirto != NULL) {
		alkuSarake = viimeSiirto->getAlkuruutu().getSarake();
		alkuRivi = viimeSiirto->getAlkuruutu().getRivi();
		loppuSarake = viimeSiirto->getLoppuruutu().getSarake();
		loppuRivi = viimeSiirto->getLoppuruutu().getRivi();
	}
	/*
		Kaksinkertainen for silmukka joka käy asema-olion laudan läpi ja jos
		siinä on nappula olio, niin kysytään sen nimi
		ja tulostetaan se muussa tapauksessa tulostetaan 2 tyhjää välilyöntiä.
		Parillisuuteen perustuen värjätään jokatoisen "ruudun" taustaväri
	*/
	for (int y = 7; y >= 0; y--) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_RED |
			FOREGROUND_GREEN | FOREGROUND_BLUE);
		wcout << y + 1 << " ";
		for (int x = 0; x < 8; x++) {
			if ((y + x) % 2 == 0) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
					FOREGROUND_INTENSITY | FOREGROUND_RED |
					FOREGROUND_GREEN | FOREGROUND_BLUE);
			}
			else {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
					BACKGROUND_RED |
					FOREGROUND_INTENSITY | FOREGROUND_RED |
					FOREGROUND_GREEN | FOREGROUND_BLUE);
			}
			if (viimeSiirto != NULL) {
				if (viimeSiirto->onkoLyhytLinna()) {
					if (asema->getSiirtovuoro()) {
						if ((x == 5 || x == 6) && y == 0) {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
								BACKGROUND_BLUE |
								FOREGROUND_INTENSITY | FOREGROUND_RED |
								FOREGROUND_GREEN | FOREGROUND_BLUE);
						}
					}
					else {
						if ((x == 5 || x == 6) && y == 7) {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
								BACKGROUND_BLUE |
								FOREGROUND_INTENSITY | FOREGROUND_RED |
								FOREGROUND_GREEN | FOREGROUND_BLUE);
						}
					}
				}else if (viimeSiirto->onkoPitkälinna()) {
					if (asema->getSiirtovuoro()) {
						if ((x == 3 || x == 2) && y == 0) {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
								BACKGROUND_BLUE |
								FOREGROUND_INTENSITY | FOREGROUND_RED |
								FOREGROUND_GREEN | FOREGROUND_BLUE);
						}
					}
					else {
						if ((x == 3 || x == 2) && y == 7) {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
								BACKGROUND_BLUE |
								FOREGROUND_INTENSITY | FOREGROUND_RED |
								FOREGROUND_GREEN | FOREGROUND_BLUE);
						}
					}
				}
				else if (x == alkuSarake && y == alkuRivi) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
						BACKGROUND_BLUE |
						FOREGROUND_INTENSITY | FOREGROUND_RED |
						FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
				else if (x == loppuSarake && y == loppuRivi) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
						BACKGROUND_BLUE |
						FOREGROUND_INTENSITY | FOREGROUND_RED |
						FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
			}

			if (asema->lauta[x][y] != nullptr) {
				wcout << " " << asema->lauta[x][y]->unicode << " ";
			}
			else {
				wcout << "   ";
			}
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_RED |
			FOREGROUND_GREEN | FOREGROUND_BLUE);
		wcout << endl;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		FOREGROUND_INTENSITY | FOREGROUND_RED |
		FOREGROUND_GREEN | FOREGROUND_BLUE);
	wcout << "   a  b  c  d  e  f  g  h ";
}
