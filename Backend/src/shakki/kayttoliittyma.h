#pragma once
#include "asema.h"
#include "nappula.h"
#include "Siirto.h"

// Shakkiohjelman käyttöliittymä, joka osaa visualisoida nykyisen aseman
// ja lukea käyttäjän syöttämät siirrot.
class Kayttoliittyma
{
public:

	Kayttoliittyma(Asema* asema) { this->asema = asema; }
	Siirto annaVastustajanSiirto();
	void piirraLauta(Siirto* viimeSiirto);

private:

	Asema* asema;
};
