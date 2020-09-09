#include "transpositionTable.h"
#include <vector>

int TranspositionTable::getIndex(long key) {
	return key & transposition_table_mask;
}

void TranspositionTable::uusiVuoro() {
	nykyVuoro++;
}

TranspositionTable::TranspositionTable() {
	nykyVuoro = 0;
	memset(items_depth_preferred, 0, sizeof(TranspositionItem) * transposition_table_size);
	memset(items_always_replace, 0, sizeof(TranspositionItem) * transposition_table_size);

}

void TranspositionTable::store(long key, Siirto parasSiirto, int syvyys, double evaluaatio, SolmuTyyppi solmuTyyppi) {
	TranspositionItem value;
	value.hash = key;
	value.parasSiirto = parasSiirto;
	value.syvyys = syvyys;
	value.evaluaatio = evaluaatio;
	value.solmuTyyppi = solmuTyyppi;
	value.vuoro = nykyVuoro;

	int index = getIndex(key);
	auto oldItem = items_depth_preferred[index];
	if (oldItem.syvyys <= syvyys || nykyVuoro != oldItem.vuoro) {
		items_depth_preferred[index] = value;
	}
	else {
		items_always_replace[index] = value;
	}
}

TranspositionItem* TranspositionTable::retrieve(long key) {
	auto item = &items_depth_preferred[getIndex(key)];
	if (item->hash == key && nykyVuoro == item->vuoro) {
		return item;
	}
	else {
		item = &items_always_replace[getIndex(key)];
		if (item->hash == key && nykyVuoro == item->vuoro) {
			return item;
		}
	}
	return nullptr;
}

TranspositionItem* TranspositionTable::retrieveOld(long key) {
	auto item = &items_depth_preferred[getIndex(key)];
	if (item->hash == key) {
		return item;
	}
	else {
		item = &items_always_replace[getIndex(key)];
		if (item->hash == key) {
			return item;
		}
	}
	return nullptr;
}

std::vector<int> TranspositionTable::checkItems(int maxDepth) {
	std::vector<int> itemsAtDepth(maxDepth);

	for (size_t i = 0; i < transposition_table_size; i++) {
		TranspositionItem item = items_depth_preferred[i];
		TranspositionItem item2 = items_always_replace[i];
		if (item.hash != 0 && nykyVuoro == item.vuoro && item.syvyys < maxDepth) {
			itemsAtDepth[item.syvyys] ++;
		}
		if (item2.hash != 0 && nykyVuoro == item2.vuoro && item2.syvyys < maxDepth) {
			itemsAtDepth[item2.syvyys] ++;
		}
	}

	return itemsAtDepth;
}
