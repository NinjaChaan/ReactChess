#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Siirto.h"

class Asema;

struct BookMove {
	Siirto siirto;
	double evaluaatio;
	std::string nimi;
};

struct OpeningNode {
	std::string nimi;
	Siirto siirto;
	std::vector<OpeningNode*> lapset;
	double evaluaatio;
	int vVoitot = 0;
	int mVoitot = 0;
	int tasapelit = 0;

	OpeningNode* HaeLapsi(Siirto siirto);
	OpeningNode* HaeSatunnainenLapsi();
	OpeningNode* HaeParasLapsi();
	OpeningNode* HaeHyvaLapsi();
	OpeningNode* HaeSuurinLapsi();
	int LaskeKaikkiLapset();
};

class OpeningBook {
private:
	std::vector<Siirto> haeSiirrot(std::string siirrot);
	std::vector<OpeningNode*> juuriSolmut;

	std::unordered_map<std::string, std::vector<BookMove>> asemaIndeksi;
	void RakennaAsemaIndeksi();
public:
	void Initialize(std::vector<std::string> filenames);
	OpeningNode* HaeJuuri(Siirto siirto);
	OpeningNode* HaeSatunnainenJuuri();
	OpeningNode* HaeParasJuuri();
	OpeningNode* HaeHyvaJuuri();
	OpeningNode* HaeSuurinJuuri();
	void Sort();

	// Every lookup key goes through this. Empty string if the fen is invalid.
	static std::string NormalisoiFen(const std::string& fen);

	const std::vector<BookMove>* HaeAsemasta(const std::string& fenAvain) const;
	bool HaeHyvaSiirto(const std::string& fenAvain, BookMove& tulos) const;

	size_t AsemienMaara() const { return asemaIndeksi.size(); }
};

