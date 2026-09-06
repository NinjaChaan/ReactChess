#pragma once

class Asema;
class Siirto;

class Hash {
public:
	static long nappulat[12][8][8];
	static long mustanVuoro;

	static long onkoValkeaKuningasLiikkunutHash;
	static long onkoMustaKuningasLiikkunutHash;
	static long onkoValkeaDTliikkunutHash;
	static long onkoValkeaKTliikkunutHash;
	static long onkoMustaDTliikkunutHash;
	static long onkoMustaKTliikkunutHash;

	static long kaksoisaskel[8];

	static void init();

	long hash;

	void rebuild(Asema* asema);
	void update(Siirto* siirto, Asema* asema);
	bool tarkista(Asema* asema);
};