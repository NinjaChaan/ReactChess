#pragma once

#include <list>
#include <string>
#include "Siirto.h"
#include "minmaxpaluu.h"
#include "lautaDict.h"
#include "hashing.h"
#include "transpositionTable.h"
#include "stats.h"
#include "siirtokello.h"
#include <thread>
#include<ctype.h>



// Ns. "forward declaration". Nyt Asema-luokassa voidaa esitell� Nappula-osoittimia ilman,
// ett� nappula.h -tiedostoa t�ytyy includoida.
class Nappula;


// Asema sis�lt�� kaiken tarvittavan informaation pelitilanteen kuvaamiseksi
// (nappuloiden sijainti, siirtovuoro, linnoitusoikeudet jne.).
class Asema {

public:
	// Pelilauta sis�lt�� osoittimet kunkin ruudun nappula-olioon (nullptr/NULL/0 jos ruutu on tyhj�).
	// Public-m��reell�, koska t�t� k�ytet��n paljon muualla.
	Nappula* lauta[8][8];

	// Nappula-oliot. Huomaa, ett� samaa nappulaa voidaan k�ytt�� useissa eri ruuduissa.
	// M��ritelty static-m��reell�, joten nappulat ovat kaikkien lauta-olioiden "yhteisk�yt�ss�"
	// (suorituskyvyn vuoksi).
	static Nappula* vk, * vd, * vt, * vl, * vr, * vs;	// Valkeat nappulat.
	static Nappula* mk, * md, * mt, * ml, * mr, * ms;	// Mustat nappulat.
	static TranspositionTable transposTable;
	static Stats stats;
	static Siirtokello siirtokello;
	static void AlustaEvaluaatio();

	// Ohestaly�nti� varten (-1 = sotilaan kaksoisaskelta ei tapahtunut edellisell� siirrolla).
	int kaksoisaskelSarakkeella = -1;

	// Siirtojen m��r�
	int vDsiirrot = 0;
	int mDsiirrot = 0;
	int vLsiirrot = 0;
	int mLsiirrot = 0;
	int vTsiirrot = 0;
	int mTsiirrot = 0;

	int nappuloidenMaara = 0;

	Hash hash;

	Asema(); // Asettaa alkuaseman.void paivitaAsema(Siirto* siirto);

	Asema(std::string fen);

	void paivitaAsema(Siirto* siirto);
	double evaluoi();																	// Aseman numeerinen arviointi.
	double evaluoiNegamax();
	double quiescent(double alpha, double beta);
	
	MinMaxPaluu iteratiivinenAlphaBeta2(const lautaDict& historia, float kello, int* maxSyvyys);
	MinMaxPaluu iterAlphaBetaRoot2(int syvyys, std::vector<MinMaxPaluu>* paluuarvot, lautaDict historia, Siirto viimeParasSiirto);
	double iterAlphaBeta2(int syvyys, int syvyysJuuresta, const lautaDict& historia, double alpha, double beta);


	bool tulossaTasapeli(const lautaDict& historia);
	bool getSiirtovuoro();
	void setSiirtovuoro(int vari);
	bool getOnkoValkeaKuningasLiikkunut();
	bool getOnkoMustaKuningasLiikkunut();
	bool getOnkoValkeaDTliikkunut();
	bool getOnkoValkeaKTliikkunut();
	bool getOnkoMustaDTliikkunut();
	bool getOnkoMustaKTliikkunut();
	std::vector<Siirto> annaRaakaSiirrot();
	std::vector<Siirto> annaRaakaSiirrotKillerTt(Siirto ttSiirto, Siirto killer1, Siirto killer2);
	std::vector<Siirto> annaLaillisetSiirrot();
	bool onkoRuutuUhattu(Ruutu ruutu);
	Ruutu getValkeaKuningasRuutu();
	Ruutu getMustaKuningasRuutu();
	Nappula* getNappulaRuudussa(int sarake, int rivi);
	int laskeNappulatVieressa(int sarake, int rivi, int vari);
	bool onkoNappulaVieressa(Ruutu ruutu, int nappula);
	bool onkoVapaaSotilas(int sarake, int rivi, int vari);
	double tuplaSotilaat(int sarake, int rivi, int vari);
	bool mahdotonLoppupeli();
	std::string getFen();

private:
	bool siirtovuoro;
	bool onkoValkeaKuningasLiikkunut;
	bool onkoMustaKuningasLiikkunut;
	bool onkoValkeaDTliikkunut;
	bool onkoValkeaKTliikkunut;
	bool onkoMustaDTliikkunut;
	bool onkoMustaKTliikkunut;
	Ruutu valkeaKuningasRuutu;
	Ruutu mustaKuningasRuutu;
	
	double laskeNappuloidenArvo();
	double paikkaEvaluointi();
	double kuninkaanTurvallisuus();
	double sotilaidenLiikeArvo();
	double nappuloitaKeskella();
	double linjat();
};
