#include "openingBook.h"
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
			cerr << "Unable to open file " << filename;
			exit(1);
		}

		string line;
		std::getline(inFile, line); //skip first line

		while (std::getline(inFile, line)) {
			lines.push_back(line);
		}
		inFile.close();
		wcout << L"done\n";
	}
	//Sort liikkeiden pituusjärjestyksessä
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
