#pragma once
#include "Ruutu.h"
#include <string>

class Asema;

class Siirto {
public:
	Siirto(Ruutu, Ruutu);
	Siirto();
	Siirto(bool lyhytLinna, bool pitkaLinna); // Poikkeussiirto linnoitusta varten
	Ruutu getAlkuruutu();
	Ruutu getLoppuruutu();
	bool onkoLyhytLinna();
	bool onkoPitkalinna();
	bool onkoEnPassant();
	int getMiksiKorotetaan();
	void setMiksiKorotetaan(int miksiKorotetaan);
	void setEnPassant(bool enPassant);
	std::wstring toString(Asema* asema);
	std::string toNormalString(Asema* asema);
	std::string toUCIString(Asema* asema);
	static Siirto fromUCIString(std::string uciMove, Asema* asema);
	bool operator==(const Siirto& other)const;
	bool operator!=(const Siirto& other)const;
	bool operator<(const Siirto& other)const;
	bool operator>(const Siirto& other)const;
	int syontiArvo = -1;
	bool syonti;
private:
	Ruutu alkuRuutu;
	Ruutu loppuRuutu;
	int miksiKorotetaan = 0;
	bool lyhytLinna;
	bool pitkalinna;
	bool enPassant;
};
