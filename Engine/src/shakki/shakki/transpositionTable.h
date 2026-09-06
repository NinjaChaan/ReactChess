#pragma once
#include "Siirto.h"
#include <array>
#include <vector>

enum SolmuTyyppi {
	tarkka,
	yla,
	ala
};
const int maxAge = 11;

// 2^24 entries is 2 GiB across the two tables; wasm32 cannot address that much.
#ifndef TT_SIZE_LOG2
#define TT_SIZE_LOG2 24
#endif
const int transposition_table_size = 1 << TT_SIZE_LOG2;
const int transposition_table_mask = transposition_table_size - 1;
struct TranspositionItem {
	long hash;
	Siirto parasSiirto;
	int syvyys;
	double evaluaatio;
	SolmuTyyppi solmuTyyppi;
	int vuoro;
};

class TranspositionTable {
private:
	TranspositionItem items_depth_preferred[transposition_table_size];
	TranspositionItem items_always_replace[transposition_table_size];
	int nykyVuoro;
	int getIndex(long key);
public:
	void uusiVuoro();
	TranspositionTable();
	void store(long key, Siirto parasSiirto, int syvyys, double evaluaatio, SolmuTyyppi solmuTyyppi);
	TranspositionItem* retrieve(long key);
	TranspositionItem* retrieveOld(long key);
	std::vector<int> checkItems(int maxDepth);
};