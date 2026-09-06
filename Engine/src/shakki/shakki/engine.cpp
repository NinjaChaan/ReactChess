#include "engine.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "asema.h"
#include "hashing.h"
#include "lautaDict.h"
#include "openingBook.h"

using namespace std;

namespace shakki {

namespace {

OpeningBook openingBook;

string asemaAvain(Asema& asema) {
	return asema.getFen() + " " + asema.getEnPassant();
}

bool haeKirjaSiirto(Asema& asema, const vector<Siirto>& lailliset, Siirto& tulos) {
	BookMove kirja;
	if (!openingBook.HaeHyvaSiirto(asemaAvain(asema), kirja)) {
		return false;
	}
	for (const Siirto& s : lailliset) {
		if (s == kirja.siirto) {
			tulos = s;
			return true;
		}
	}
	return false;
}

Siirto haeSiirtoHaulla(Asema& asema, int maxSyvyys) {
	using clock = std::chrono::system_clock;
	using sec = std::chrono::duration<double>;

	lautaDict fake;
	int saavutettuSyvyys = 0;
	auto start = clock::now();

	MinMaxPaluu paras = asema.iteratiivinenAlphaBeta2(fake, 1, maxSyvyys, &saavutettuSyvyys);

#ifdef SHAKKI_VERBOSE
	wcout << endl << "Koneen suosittelema siirto: " << endl;
	wprintf(L"%ls : %.2f\n", paras._parasSiirto.toString(&asema).c_str(), paras._evaluointiArvo);
	const sec duration = clock::now() - start;
	wprintf(L"Evaluointi kesti %.2fs. Syvyys: %d \n", duration.count(), saavutettuSyvyys - 1);

	std::vector<int> items = asema.transposTable.checkItems(saavutettuSyvyys);
	for (size_t i = 0; i < items.size(); i++) {
		wcout << "Depth " << i << ": " << items[i] << endl;
	}
	asema.stats.Print();
#else
	(void)start;
#endif
	asema.stats.Reset();

	return paras._parasSiirto;
}

int syvyysVaikeudesta(int difficulty) {
	switch (difficulty) {
	case 1: return 2;
	case 2: return 4;
	case 3: return 30;
	default: return 2;
	}
}

string voittajaTeksti(Asema& asema) {
	return asema.getSiirtovuoro() == 1 ? "White wins" : "Black wins";
}

}

void init() {
	Hash::init();
	srand(time(NULL));
	Asema::AlustaEvaluaatio();

	vector<string> bookFiles = { "./db/a.tsv", "./db/b.tsv", "./db/c.tsv", "./db/d.tsv", "./db/e.tsv" };
	openingBook.Initialize(bookFiles);
}

TurnResult playTurn(const string& fen, int difficulty) {
	TurnResult tulos;
	tulos.enPassant = "-";

	Asema asema(fen);
	vector<Siirto> siirtolista = asema.annaLaillisetSiirrot();

	if (siirtolista.size() == 0) {
		tulos.fen = voittajaTeksti(asema);
		return tulos;
	}
	if (asema.mahdotonLoppupeli()) {
		tulos.fen = "Draw";
		return tulos;
	}

	Siirto valittu;
	if (!haeKirjaSiirto(asema, siirtolista, valittu)) {
		valittu = haeSiirtoHaulla(asema, syvyysVaikeudesta(difficulty));
	}

	tulos.lastMove = valittu.toNormalString(&asema);
	asema.paivitaAsema(&valittu);

	for (auto s : asema.annaLaillisetSiirrot()) {
		tulos.legalMoves.push_back(s.toNormalString(&asema));
	}
	if (tulos.legalMoves.size() == 0) {
		tulos.fen = voittajaTeksti(asema);
		return tulos;
	}

	tulos.fen = asema.getFen();
	tulos.enPassant = asema.getEnPassant();
	return tulos;
}

vector<string> getLegalMoves(const string& fen) {
	vector<string> laillisetSiirrot;
	Asema asema(fen);

	for (auto s : asema.annaLaillisetSiirrot()) {
		laillisetSiirrot.push_back(s.toNormalString(&asema));
	}

	return laillisetSiirrot;
}

string getBestMove(const string& fen) {
	Asema asema(fen);
	vector<Siirto> siirtolista = asema.annaLaillisetSiirrot();
	if (siirtolista.size() == 0) {
		return "";
	}

	Siirto valittu;
	if (!haeKirjaSiirto(asema, siirtolista, valittu)) {
		valittu = haeSiirtoHaulla(asema, 30);
	}

	return valittu.toNormalString(&asema);
}

string isGameOver(const string& fen) {
	Asema asema(fen);
	vector<Siirto> siirtolista = asema.annaLaillisetSiirrot();

	if (siirtolista.size() == 0) {
		return voittajaTeksti(asema);
	}
	if (asema.mahdotonLoppupeli()) {
		return "Draw";
	}
	return "Continue";
}

}
