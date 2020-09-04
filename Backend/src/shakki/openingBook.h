#pragma once
#include <string>
#include <vector>
#include "Siirto.h"

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
public:
	void Initialize(std::vector<std::string> filenames);
	OpeningNode* HaeJuuri(Siirto siirto);
	OpeningNode* HaeSatunnainenJuuri();
	OpeningNode* HaeParasJuuri();
	OpeningNode* HaeHyvaJuuri();
	OpeningNode* HaeSuurinJuuri();
	void Sort();
};

