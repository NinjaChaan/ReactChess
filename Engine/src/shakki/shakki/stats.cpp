#include "stats.h"
#include <iostream>

using namespace std;

void Stats::Print() {
	wcout << "Asemia kokeiltu: " << asemiaKokeiltu << endl;
	wcout << "Tttaulun osumat: " << ttTauluOsumat << endl;
	wcout << "Tttaulu oikein: " << ttParasOikein << endl;
	wcout << "Killer oikein: " << killerOikein << endl;
}

void Stats::Reset() {
	asemiaKokeiltu = 0;
	ttTauluOsumat = 0;
	ttParasOikein = 0;
	killerOikein = 0;
}
