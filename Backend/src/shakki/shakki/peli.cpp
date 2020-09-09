#include <iostream>
#include "peli.h"
#include "kayttoliittyma.h"
#include "asema.h"
#include <list>
#include <chrono>
#include <algorithm>
#include <locale>
#include <codecvt>

using namespace std;

int Peli::siirtoVuoro = 0;
enum Pelimoodi {
	automaatti,
	yksinpeli,
	kaksinpeli
};

#define Randomize false

Peli::Peli(int ihmisenVari) {
	Hash::init();
	srand(time(NULL));
	using clock = std::chrono::system_clock;
	using sec = std::chrono::duration<double>;

	OpeningBook openingBook;
	Asema asema;
	Asema::AlustaEvaluaatio();
	Kayttoliittyma ui(&asema);

	std::vector<std::string> bookFiles = { "./db/a.tsv"
		, "./db/b.tsv", "./db/c.tsv", "./db/d.tsv", "./db/e.tsv" };
	//vector<string> bookFiles = { "./db/white.tsv", "./db/black.tsv" };
	openingBook.Initialize(bookFiles);

	vector<Siirto> siirtolista;
	Pelimoodi moodi;

	Siirto* viimeSiirto = NULL;

	int n = -1;
	while (n != 0 && n != 1 && n != 2) {
		wcout << "Monta ihmispelaajaa? (0/1/2) ";
		cin >> n;
	}

	moodi = static_cast<Pelimoodi>(n);

	char v = ' ';
	int pelaajanVari = -1;
	if (n == 1) {
		while (v != 'V' && v != 'M') {
			wcout << "Milla varilla ihminen pelaa? (V/M) ";
			cin >> v;
		}
		pelaajanVari = v == 'V' ? 0 : 1;
	}
	else {
		pelaajanVari = 0;
	}

	OpeningNode* openingNode = nullptr;
	int vuoro = 0;
	while (true) {
		Asema::transposTable.uusiVuoro();
		siirtoVuoro = asema.getSiirtovuoro();
		wcout << endl;
		ui.piirraLauta(viimeSiirto);
		wcout << endl;
		siirtolista = asema.annaLaillisetSiirrot();

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		fen = asema.getFen();
		std::wstring wide = converter.from_bytes(fen);
		wcout << wide << endl;


		if (onkoTasapeli() || asema.mahdotonLoppupeli()) {
			wcout << "Tasapeli." << endl;
			break;
		}

		if (siirtolista.size() == 0) {
			wprintf(L"%s voitti!", asema.getSiirtovuoro() == 1 ? L"Valkoinen" : L"Musta");
			break;
		}

		auto start = clock::now();

		wprintf(L"Aseman evaluaatio: %.2f\n", asema.evaluoi());
		wcout << "Sallitut siirrot: " << endl;
		int i = 0;
		/*for (Siirto s : siirtolista) {
			if (i++ == 5) { wcout << endl; i = 0; }
			wcout << s.toString(&asema) << "   ";
		}*/
		bool laillinen = false;
		Siirto s;
		int syvyys = 6;
		if (asema.nappuloidenMaara < 10) {
			syvyys += 1;
		}
		if (asema.nappuloidenMaara < 5) {
			syvyys += 1;
		}

		int maxSyvyys = syvyys + 1;
		MinMaxPaluu paras;
		OpeningNode* uusiNode = nullptr;

		if (vuoro == 0) {
			if (Randomize) {
				uusiNode = openingBook.HaeSatunnainenJuuri();
			}
			else {
				uusiNode = openingBook.HaeHyvaJuuri();
			}
		}
		else if (openingNode != nullptr) {
			if (Randomize) {
				uusiNode = openingNode->HaeSatunnainenLapsi();
			}
			else {
				uusiNode = openingNode->HaeHyvaLapsi();
			}
		}
		if (uusiNode == nullptr) {
			paras = asema.iteratiivinenAlphaBeta2(asemaHistoria, 5, &maxSyvyys);

			wcout << endl << "Koneen suosittelema siirto: " << endl;
			wprintf(L"%s : %.2f\n", paras._parasSiirto.toString(&asema).c_str(), paras._evaluointiArvo);
			const sec duration = clock::now() - start;
			wprintf(L"Evaluointi kesti %.2fs. Syvyys: %d \n", duration.count(), maxSyvyys - 1);

			std::vector<int> items = asema.transposTable.checkItems(maxSyvyys);

			for (size_t i = 0; i < items.size(); i++) {
				wcout << "Depth " << i << ": " << items[i] << endl;
			}
			asema.stats.Print();
			asema.stats.Reset();
		}
		else {
			wstring uusiNodeNimi = wstring(uusiNode->nimi.begin(), uusiNode->nimi.end());
			wcout << L"Opening Book: " << uusiNodeNimi << L" : " << uusiNode->siirto.toString(&asema);
		}

		if (moodi == Pelimoodi::kaksinpeli || (moodi == Pelimoodi::yksinpeli && asema.getSiirtovuoro() == pelaajanVari)) {
			while (!laillinen) {
				s = ui.annaVastustajanSiirto();

				for (Siirto siirto : siirtolista) {
					if (s == siirto) {
						s = siirto;
						laillinen = true;
						break;
					}
				}
			}
		}
		else {
			if (uusiNode != nullptr) {
				s = uusiNode->siirto;
			}
			else {
				s = paras._parasSiirto;
			}
		}
		viimeSiirto = &s;
		asema.paivitaAsema(&s);
		lisaaAsemaHistoriaan(&asema);


		//Päivitä opening book
		OpeningNode* actual = nullptr;
		if (vuoro == 0) {
			actual = openingBook.HaeJuuri(s);
		}
		else if (openingNode != nullptr) {
			actual = openingNode->HaeLapsi(s);
		}

		if (actual != nullptr && uusiNode != nullptr && actual != uusiNode) {
			wstring uusiNodeNimi = wstring(actual->nimi.begin(), actual->nimi.end());
			wcout << "Unexpected opening: " << uusiNodeNimi << endl;
		}
		openingNode = actual;

		vuoro++;
	}
}

int Peli::getKoneenVari() {
	return 0;
}

bool Peli::onkoTasapeli() {
	for (auto kvp : this->asemaHistoria) {
		if (kvp.second == 3) return true;
	}
	return false;
}

void Peli::lisaaAsemaHistoriaan(Asema* asema) {
	Nappula** lautaCopy = new Nappula * [64];
	Nappula** lauta = (Nappula**)asema->lauta;
	memcpy(lautaCopy, lauta, sizeof(void*) * 64);

	auto found = asemaHistoria.find(lautaCopy);
	if (found != asemaHistoria.end()) {
		found->second = found->second + 1;
	}
	else {
		asemaHistoria[lautaCopy] = 1;
	}
}

