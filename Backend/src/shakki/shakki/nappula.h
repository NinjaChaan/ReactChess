#pragma once
#include <list>
#include <vector>
#include <string>
#include "asema.h"
#include "Siirto.h"

// Vakioarvot nappulatyypeille.
enum {
	VT, VR, VL, VD, VK, VS,
	MT, MR, ML, MD, MK, MS
};


class Nappula {
protected:
	void talletaSiirto(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari, int xSiirto, int ySiirto);
	void liikuRatsu(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari, int xSuunta, int ySuunta);
	void liikuSuorassa(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari, int xSuunta, int ySuunta);
private:
public:
	int _koodi; // VT, VR, MT tms.
	double arvo;
	std::wstring unicode;
	int vari; // valkea = 0, musta = 1
	Nappula(std::wstring unicode, int vari, int koodi, double arvo) {
		this->unicode = unicode;
		this->vari = vari;
		this->_koodi = koodi;
		this->arvo = arvo;
	};
	Nappula() {};

	virtual void annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu*, Asema*, int vari) = 0; // virtual jotta myˆh‰inen sidonta k‰ytˆss‰


//	void setNimi(std::wstring);
//	std::wstring getNimi();
	void setUnicode(std::wstring unicode) { this->unicode = unicode; }
	std::wstring getUnicode() { return unicode; };
	void setVari(int vari) { this->vari = vari; }
	int getVari() { return vari; };
	int getKoodi() { return _koodi; };
	void setKoodi(int koodi) { _koodi = koodi; }
	double getArvo() { return arvo; };
};

class Torni : public virtual Nappula { // virtual = kantaluokka perit‰‰n moniperinn‰ss‰ vain kerran!
public:
	Torni(std::wstring unicode, int vari, int koodi, double arvo) : Nappula(unicode, vari, koodi, arvo) {} // alustuslista (yliluokan alustus)
	void annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari);
};

class Ratsu : public Nappula {
public:
	Ratsu(std::wstring unicode, int vari, int koodi, double arvo) : Nappula(unicode, vari, koodi, arvo) {}
	void annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari);
};

class Lahetti : public virtual Nappula { // virtual = kantaluokka perit‰‰n moniperinn‰ss‰ vain kerran!
public:
	Lahetti(std::wstring unicode, int vari, int koodi, double arvo) : Nappula(unicode, vari, koodi, arvo) {}
	void annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari);
};

class Daami : public Lahetti, public Torni { // Moniperint‰!
public:
	Daami(std::wstring unicode, int vari, int koodi, double arvo) : Nappula(unicode, vari, koodi, arvo), Lahetti(unicode, vari, koodi, arvo), Torni(unicode, vari, koodi, arvo) {
		//setUnicode(unicode);
		//setVari(vari);
		//setKoodi(koodi);
	}
	void annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari);
};

class Kuningas : public Nappula {
public:
	Kuningas(std::wstring unicode, int vari, int koodi, double arvo) : Nappula(unicode, vari, koodi, arvo) {}
	void annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari);
};

class Sotilas : public Nappula {
public:
	Sotilas(std::wstring unicode, int vari, int koodi, double arvo) : Nappula(unicode, vari, koodi, arvo) {}
	void annaSiirrot(std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Ruutu* ruutu, Asema* asema, int vari);
private:
	void lisaaSotilaanKorotukset(Siirto*, std::vector<Siirto>& siirrot, std::vector<Siirto>& syonnit, Asema* asema, int vari);
};
