#include "openingBook.h"
#include "asema.h"
#include <fstream>
#include <iostream>
#include "stringSplit.h"
#include <algorithm>

using namespace std;


std::vector<Siirto> OpeningBook::haeSiirrot(std::string siirrotString) {
	vector<Siirto> siirrot;
	vector<string> parts;
	stringSplit(siirrotString, parts, ' ');
	for (string s : parts) {
		Siirto siirto;
		if (s == "e1g1" || s == "e8g8") {
			siirto = Siirto(true, false);
		}
		else if (s == "e1c1" || s == "e8c8") {
			siirto = Siirto(false, true);
		}
		else {
			char r1c = s.c_str()[0] - 'a';
			char r1r = s.c_str()[1] - '1';
			char r2c = s.c_str()[2] - 'a';
			char r2r = s.c_str()[3] - '1';
			Ruutu ruutu1(r1c, r1r);
			Ruutu ruutu2(r2c, r2r);
			siirto = Siirto(ruutu1, ruutu2);
		}
		siirrot.push_back(siirto);
	}
	return siirrot;
}


bool greaterMoves(const string& s1, const string& s2) {
	vector<string> parts1;
	stringSplit(s1, parts1, '\t');
	string siirtoStr1 = parts1[3];

	vector<string> parts2;
	stringSplit(s2, parts2, '\t');
	string siirtoStr2 = parts2[3];

	return siirtoStr1 < siirtoStr2;
}

void OpeningBook::Initialize(std::vector<std::string> filenames) {
	wcout << L"Reading opening books..." << endl;
	vector<string> lines;
	
	for (string filename : filenames) {
		ifstream inFile;

		std::wstring filenameW(filename.begin(), filename.end());
		wcout << filenameW << L"...   ";

		inFile.open(filename);
		if (!inFile) {
			cerr << "Unable to open file " << filename << ", skipping\n";
			continue;
		}

		string line;
		std::getline(inFile, line); //skip first line

		while (std::getline(inFile, line)) {
			lines.push_back(line);
		}
		inFile.close();
		wcout << L"done\n";
	}
	//Sort liikkeiden pituusj�rjestyksess�
	std::sort(lines.begin(), lines.end(), greaterMoves);
	for (string line : lines) {

		vector<string> parts;
		stringSplit(line, parts, '\t');
		string siirtoStr = parts[3];
		auto siirrot = haeSiirrot(siirtoStr);

		int vVoitot = std::stoi(parts[4]);
		int mVoitot = std::stoi(parts[5]);
		int tasapelit = std::stoi(parts[6]);
		int kaikkiPelit = vVoitot + mVoitot + tasapelit;

		if (kaikkiPelit < 100) 
			continue;

		double vPros = (double)vVoitot / kaikkiPelit;
		double mPros = (double)mVoitot / kaikkiPelit;
		double tPros = (double)tasapelit / kaikkiPelit;

		OpeningNode* juuri = HaeJuuri(siirrot[0]);
		if (juuri == nullptr) {
			juuri = new OpeningNode();
			juuri->nimi = parts[1];
			juuri->siirto = siirrot[0];
			juuri->vVoitot = vVoitot;
			juuri->mVoitot = mVoitot;
			juuri->tasapelit = tasapelit;
			juuri->evaluaatio = vPros + (tPros / 2.0);

			juuriSolmut.push_back(juuri);
		}
		OpeningNode* edellinen = juuri;
		for (int i = 1; i < siirrot.size(); i++) {
			OpeningNode* seuraava = edellinen->HaeLapsi(siirrot[i]);
			if (seuraava == nullptr) {
				seuraava = new OpeningNode();
				seuraava->nimi = parts[1];
				seuraava->siirto = siirrot[i];
				seuraava->vVoitot = vVoitot;
				seuraava->mVoitot = mVoitot;
				seuraava->tasapelit = tasapelit;
				

				if (i % 2 == 0) {
					seuraava->evaluaatio = vPros + (tPros / 2.0);
				}
				else {
					seuraava->evaluaatio = mPros + (tPros / 2.0);
				}

				edellinen->lapset.push_back(seuraava);

				/*if (i % 2 == 0 && std::find(whiteLines.begin(), whiteLines.end(), line) != whiteLines.end()) {
					juuri->hyvaSiirto = true;
				}
				else if (i % 2 == 1 && std::find(blackLines.begin(), blackLines.end(), line) != blackLines.end()) {
					juuri->hyvaSiirto = true;
				}
				else {
					juuri->hyvaSiirto = false;
				}*/
			}
			edellinen = seuraava;
		}
	}


	Sort();
	RakennaAsemaIndeksi();
}


std::string OpeningBook::NormalisoiFen(const std::string& fen) {
	// getFen() omits the en passant square, the tsv fen column includes it.
	// Round-tripping both through Asema makes the two agree.
	try {
		Asema asema(fen);
		return asema.getFen() + " " + asema.getEnPassant();
	}
	catch (...) {
		return "";
	}
}


static void indeksoiSolmut(Asema asema, const std::vector<OpeningNode*>& solmut,
	std::unordered_map<std::string, std::vector<BookMove>>& indeksi) {

	const std::string avain = asema.getFen() + " " + asema.getEnPassant();
	std::vector<Siirto> lailliset = asema.annaLaillisetSiirrot();

	for (OpeningNode* solmu : solmut) {
		// haeSiirrot() only reads the from/to squares, so castling, en passant
		// and promotion flags come from the matching legal move.
		const Siirto* laillinen = nullptr;
		for (const Siirto& s : lailliset) {
			if (s == solmu->siirto) {
				laillinen = &s;
				break;
			}
		}
		if (laillinen == nullptr) {
			continue;
		}

		std::vector<BookMove>& siirrot = indeksi[avain];
		bool jo = false;
		for (const BookMove& bm : siirrot) {
			if (bm.siirto == *laillinen) {
				jo = true;
				break;
			}
		}
		if (!jo) {
			BookMove bm;
			bm.siirto = *laillinen;
			bm.evaluaatio = solmu->evaluaatio;
			bm.nimi = solmu->nimi;
			siirrot.push_back(bm);
		}

		if (!solmu->lapset.empty()) {
			Asema seuraava = asema;
			Siirto s = *laillinen;
			seuraava.paivitaAsema(&s);
			indeksoiSolmut(seuraava, solmu->lapset, indeksi);
		}
	}
}


void OpeningBook::RakennaAsemaIndeksi() {
	// The book is a move-sequence tree, but lookups only have a position, so
	// replay it from the start and index by position instead.
	asemaIndeksi.clear();
	if (juuriSolmut.empty()) {
		return;
	}

	Asema alkuasema;
	indeksoiSolmut(alkuasema, juuriSolmut, asemaIndeksi);

	for (auto& pari : asemaIndeksi) {
		std::sort(pari.second.begin(), pari.second.end(),
			[](const BookMove& a, const BookMove& b) { return a.evaluaatio > b.evaluaatio; });
	}

	wcout << L"Opening book: " << asemaIndeksi.size() << L" positions indexed" << endl;
}


const std::vector<BookMove>* OpeningBook::HaeAsemasta(const std::string& fenAvain) const {
	auto it = asemaIndeksi.find(fenAvain);
	if (it == asemaIndeksi.end() || it->second.empty()) {
		return nullptr;
	}
	return &it->second;
}


bool OpeningBook::HaeHyvaSiirto(const std::string& fenAvain, BookMove& tulos) const {
	const std::vector<BookMove>* siirrot = HaeAsemasta(fenAvain);
	if (siirrot == nullptr) {
		return false;
	}
	int hyvat = std::max((int)(siirrot->size() * 0.3), 1);
	tulos = (*siirrot)[rand() % hyvat];
	return true;
}


OpeningNode* OpeningBook::HaeJuuri(Siirto siirto) {
	for (auto juuri : juuriSolmut) {
		if (juuri->siirto == siirto) return juuri;
	}
	return nullptr;
}

OpeningNode* OpeningBook::HaeSatunnainenJuuri() {
	if (juuriSolmut.size() == 0)
		return nullptr;
	return juuriSolmut[rand() % juuriSolmut.size()];
}

OpeningNode* OpeningBook::HaeParasJuuri() {
	if (juuriSolmut.size() == 0)
		return nullptr;

	return juuriSolmut[0];
}

OpeningNode* OpeningBook::HaeHyvaJuuri() {
	if (juuriSolmut.size() == 0)
		return nullptr;

	int hyvat = std::max((int)(juuriSolmut.size() * 0.3), 1);
	return juuriSolmut[rand() % hyvat];
}

OpeningNode* OpeningBook::HaeSuurinJuuri() {
	if (juuriSolmut.size() == 0)
		return nullptr;
	int suurinLapsimaara = -1;
	OpeningNode* suurin = nullptr;
	for (OpeningNode* juuri : juuriSolmut) {
		if ((int)juuri->lapset.size() > suurinLapsimaara) {
			suurinLapsimaara = juuri->lapset.size();
			suurin = juuri;
		}
	}
	return suurin;
}

OpeningNode* OpeningNode::HaeLapsi(Siirto siirto) {
	for (auto lapsi : lapset) {
		if (lapsi->siirto == siirto) return lapsi;
	}
	return nullptr;
}

OpeningNode* OpeningNode::HaeSatunnainenLapsi() {
	if (lapset.size() == 0)
		return nullptr;
	vector<OpeningNode*> jatkuvatLapset;
	for (OpeningNode* lapsi : lapset) {
		if (lapsi->lapset.size() >= 1) {
			jatkuvatLapset.push_back(lapsi);
		}
	}
	if (jatkuvatLapset.size() == 0) {
		return lapset[rand() % lapset.size()];
	}
	return jatkuvatLapset[rand() % jatkuvatLapset.size()];
}

OpeningNode* OpeningNode::HaeParasLapsi() {
	if (lapset.size() == 0)
		return nullptr;

	return lapset[0];
}

OpeningNode* OpeningNode::HaeHyvaLapsi() {
	if (lapset.size() == 0)
		return nullptr;

	int hyvat = std::max((int)(lapset.size() * 0.3), 1);
	return lapset[rand() % hyvat];
}


OpeningNode* OpeningNode::HaeSuurinLapsi() {
	if (lapset.size() == 0)
		return nullptr;
	int suurinLapsimaara = -1;
	OpeningNode* suurin = nullptr;
	for (OpeningNode* lapsi : lapset) {
		if ((int)lapsi->lapset.size() > suurinLapsimaara) {
			suurinLapsimaara = lapsi->lapset.size();
			suurin = lapsi;
		}
	}
	return suurin;
}

int OpeningNode::LaskeKaikkiLapset() {
	int kaikki = 0;
	for (auto node : lapset) {
		kaikki++;
		kaikki += node->LaskeKaikkiLapset();
	}
	return kaikki;
}

bool CompareNodes(OpeningNode* node1, OpeningNode* node2) {
	double evalPlus1 = node1->LaskeKaikkiLapset() * 0.0001;
	double evalPlus2 = node2->LaskeKaikkiLapset() * 0.0001;

	return (node1->evaluaatio + evalPlus1) > (node2->evaluaatio + evalPlus2);
}

void SortNodes(std::vector<OpeningNode*>& nodes) {
	for (auto node : nodes) {
		SortNodes(node->lapset);
	}
	std::sort(nodes.begin(), nodes.end(), CompareNodes);
}

void OpeningBook::Sort() {
	SortNodes(juuriSolmut);
}
