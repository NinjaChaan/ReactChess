#pragma once
#include "nappula.h"
#include <unordered_map>

class EqualFn {
public:
	bool operator() (Nappula** t1, Nappula** t2) const {
		for (size_t i = 0; i < 64; i++) {
			if (t1[i] != t2[i]) return false;
		}
		return true;
	}
};

class Hasher {
public:
	size_t operator() (Nappula** lauta) const {
		size_t h = 0;
		for (size_t i = 0; i < 64; i++) {
			if (lauta[i] != NULL) {
				Nappula* nappula = lauta[i];
				size_t nappuhash = (size_t)nappula;
				h = (h * 397) ^ (nappuhash * (i + 1));
			}
		}
		return h;
	}
};

typedef std::unordered_map<Nappula**, int, Hasher, EqualFn> lautaDict;