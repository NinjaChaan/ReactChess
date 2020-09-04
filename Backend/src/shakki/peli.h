#pragma once
#include <vector>
#include "asema.h"
#include "lautaDict.h"
#include "openingBook.h"

// Peli tietokonetta vastaan joko mustilla tai valkeilla.
class Peli
{
public:
	Peli(int);
	int getKoneenVari();
	bool onkoTasapeli();
	lautaDict asemaHistoria;
	static int siirtoVuoro;
private:
	void lisaaAsemaHistoriaan(Asema* asema);
	int _koneenVari; // Valkoinen = 0, Musta = 1
};
