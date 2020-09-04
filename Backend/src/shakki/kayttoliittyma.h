#pragma once
#include "asema.h"
#include "nappula.h"
#include "Siirto.h"

// Shakkiohjelman k�ytt�liittym�, joka osaa visualisoida nykyisen aseman
// ja lukea k�ytt�j�n sy�tt�m�t siirrot.
class Kayttoliittyma
{
public:

	Kayttoliittyma(Asema* asema) { this->asema = asema; }
	Siirto annaVastustajanSiirto();
	void piirraLauta(Siirto* viimeSiirto);

private:

	Asema* asema;
};
