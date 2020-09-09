#include <iostream>
#include "asema.h"
#include "nappula.h"
#include <algorithm>
#include "peli.h"
#include <string>
#include <exception>

Nappula *Asema::vk = new Kuningas(L"\u265A", 0, VK, 200);
Nappula *Asema::vd = new Daami(L"\u265B", 0, VD, 9);
Nappula *Asema::vt = new Torni(L"\u265C", 0, VT, 5);
Nappula *Asema::vl = new Lahetti(L"\u265D", 0, VL, 3.25);
Nappula *Asema::vr = new Ratsu(L"\u265E", 0, VR, 3);
Nappula *Asema::vs = new Sotilas(L"\u265F", 0, VS, 1);

Nappula *Asema::mk = new Kuningas(L"\u2654", 1, MK, -200);
Nappula *Asema::md = new Daami(L"\u2655", 1, MD, -9);
Nappula *Asema::mt = new Torni(L"\u2656", 1, MT, -5);
Nappula *Asema::ml = new Lahetti(L"\u2657", 1, ML, -3.25);
Nappula *Asema::mr = new Ratsu(L"\u2658", 1, MR, -3);
Nappula *Asema::ms = new Sotilas(L"\u2659", 1, MS, -1);

TranspositionTable Asema::transposTable;
Stats Asema::stats;
Siirtokello Asema::siirtokello;

const int killerMaxDepth = 20;

std::array<Siirto, killerMaxDepth> killerMoves;
std::array<Siirto, killerMaxDepth> killerMoves2;
bool killervuoro = false;

void StoreKillerMove(Siirto siirto, int depthFromRoot)
{
	auto killer1 = killerMoves[depthFromRoot];
	auto killer2 = killerMoves2[depthFromRoot];
	if (depthFromRoot < killerMaxDepth)
	{
		if (killer1 != siirto && killer2 != siirto)
		{
			if (killervuoro)
			{
				killerMoves[depthFromRoot] = siirto;
			}
			else
			{
				killerMoves2[depthFromRoot] = siirto;
			}
			killervuoro = !killervuoro;
		}
	}
}

bool GetKillerMove(Siirto *siirto, int depthFromRoot, bool toinen)
{
	if (depthFromRoot < killerMaxDepth)
	{
		if (toinen)
		{
			*siirto = killerMoves2[depthFromRoot];
			return true;
		}
		*siirto = killerMoves[depthFromRoot];
		return true;
	}
	return false;
}

__forceinline void TarkistaLiikkunut(int sarake, int rivi, Siirto &siirto, bool &liikkunut)
{
	if (!liikkunut)
	{
		if ((rivi == siirto.getAlkuruutu().getRivi() && sarake == siirto.getAlkuruutu().getSarake()) ||
			(rivi == siirto.getLoppuruutu().getRivi() && sarake == siirto.getLoppuruutu().getSarake()))
		{
			liikkunut = true;
		}
	}
}

Asema::Asema()
{
	// Ensin alustetaan kaikki laudan ruudut nappulla "NULL", koska muuten ruuduissa satunnaista tauhkaa
	for (size_t x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (y == 1)
			{
				lauta[x][y] = vs;
			}
			else if (y == 6)
			{
				lauta[x][y] = ms;
			}
			else
			{
				lauta[x][y] = nullptr;
			}
		}
	}
	// Asetetaan alkuaseman mukaisesti nappulat ruuduille
	lauta[0][0] = lauta[7][0] = vt;
	lauta[1][0] = lauta[6][0] = vr;
	lauta[2][0] = lauta[5][0] = vl;
	lauta[3][0] = vd;
	lauta[4][0] = vk;
	valkeaKuningasRuutu = Ruutu(4, 0);

	lauta[0][7] = lauta[7][7] = mt;
	lauta[1][7] = lauta[6][7] = mr;
	lauta[2][7] = lauta[5][7] = ml;
	lauta[3][7] = md;
	lauta[4][7] = mk;
	mustaKuningasRuutu = Ruutu(4, 7);

	siirtovuoro = false;
	onkoValkeaKuningasLiikkunut = false;
	onkoMustaKuningasLiikkunut = false;
	onkoValkeaDTliikkunut = false;
	onkoValkeaKTliikkunut = false;
	onkoMustaDTliikkunut = false;
	onkoMustaKTliikkunut = false;

	hash.rebuild(this);
}

Nappula *fenToPiece(char c)
{
	switch (c)
	{
	case 'K':
	{
		return Asema::vk;
		break;
	}
	case 'Q':
	{
		return Asema::vd;
		break;
	}
	case 'R':
	{
		return Asema::vt;
		break;
	}
	case 'B':
	{
		return Asema::vl;
		break;
	}
	case 'N':
	{
		return Asema::vr;
		break;
	}
	case 'P':
	{
		return Asema::vs;
		break;
	}
	case 'k':
	{
		return Asema::mk;
		break;
	}
	case 'q':
	{
		return Asema::md;
		break;
	}
	case 'r':
	{
		return Asema::mt;
		break;
	}
	case 'b':
	{
		return Asema::ml;
		break;
	}
	case 'n':
	{
		return Asema::mr;
		break;
	}
	case 'p':
	{
		return Asema::ms;
		break;
	}
	default:
		return NULL;
		break;
	}
}

Asema::Asema(std::string fen)
{
	int j = 0;
	int i = 0;
	bool wKAllowed = false;
	bool wQAllowed = false;
	bool bKAllowed = false;
	bool bQAllowed = false;

	for (char c : fen)
	{
		if (j < 8)
		{
			if (c != '/' && c != ' ')
			{
				if (isdigit(c))
				{
					for (size_t m = 0; m < c - '0'; m++)
					{
						lauta[i][7 - j] = NULL;
						i++;
					}
				}
				else
				{
					if (c == 'K')
					{
						valkeaKuningasRuutu = Ruutu(i, 7 - j);
					}
					else if (c == 'k')
					{
						mustaKuningasRuutu = Ruutu(i, 7 - j);
					}
					lauta[i][7 - j] = fenToPiece(c);
					i++;
				}
			}
			else
			{
				i = 0;
				j++;
			}
		}
		else
		{
			if (c == 'w')
			{
				setSiirtovuoro(0);
			}
			else if (c == 'b')
			{
				setSiirtovuoro(1);
			}
			else if (c == 'K')
			{
				wKAllowed = true;
			}
			else if (c == 'Q')
			{
				wQAllowed = true;
			}
			else if (c == 'k')
			{
				bKAllowed = true;
			}
			else if (c == 'q')
			{
				bQAllowed = true;
			}
		}
	}

	onkoValkeaKuningasLiikkunut = !(wKAllowed || wQAllowed);
	onkoMustaKuningasLiikkunut = !(bKAllowed || bQAllowed);
	onkoValkeaDTliikkunut = !wQAllowed;
	onkoValkeaKTliikkunut = !wKAllowed;
	onkoMustaDTliikkunut = !bQAllowed;
	onkoMustaKTliikkunut = !bKAllowed;

	hash.rebuild(this);
}

void Asema::paivitaAsema(Siirto *siirto)
{
	hash.update(siirto, this);
	if (!siirto->onkoLyhytLinna() && !siirto->onkoPitkalinna())
	{
		if (lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] == vk)
		{
			valkeaKuningasRuutu = Ruutu(-1, -1);
		}
		if (lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] == mk)
		{
			mustaKuningasRuutu = Ruutu(-1, -1);
		}

		lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] =
			lauta[siirto->getAlkuruutu().getSarake()][siirto->getAlkuruutu().getRivi()];
		lauta[siirto->getAlkuruutu().getSarake()][siirto->getAlkuruutu().getRivi()] = NULL;

		TarkistaLiikkunut(0, 7, *siirto, onkoMustaDTliikkunut);
		TarkistaLiikkunut(7, 7, *siirto, onkoMustaKTliikkunut);
		TarkistaLiikkunut(4, 7, *siirto, onkoMustaKuningasLiikkunut);

		TarkistaLiikkunut(0, 0, *siirto, onkoValkeaDTliikkunut);
		TarkistaLiikkunut(7, 0, *siirto, onkoValkeaKTliikkunut);
		TarkistaLiikkunut(4, 0, *siirto, onkoValkeaKuningasLiikkunut);

		if (lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] == vk)
		{
			valkeaKuningasRuutu = siirto->getLoppuruutu();
		}
		if (lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] == mk)
		{
			mustaKuningasRuutu = siirto->getLoppuruutu();
		}
	}
	else if (siirto->onkoLyhytLinna())
	{
		if (siirtovuoro == 0)
		{ //valkoinen
			lauta[6][0] = vk;
			lauta[5][0] = vt;
			lauta[4][0] = NULL;
			lauta[7][0] = NULL;

			onkoValkeaKuningasLiikkunut = true;
			onkoValkeaKTliikkunut = true;

			valkeaKuningasRuutu = Ruutu(6, 0);
		}
		else if (siirtovuoro == 1)
		{ //musta
			lauta[6][7] = mk;
			lauta[5][7] = mt;
			lauta[4][7] = NULL;
			lauta[7][7] = NULL;

			onkoMustaKuningasLiikkunut = true;
			onkoMustaKTliikkunut = true;

			mustaKuningasRuutu = Ruutu(6, 7);
		}
	}
	else if (siirto->onkoPitkalinna())
	{
		if (siirtovuoro == 0)
		{ //valkoinen
			lauta[2][0] = vk;
			lauta[3][0] = vt;
			lauta[4][0] = NULL;
			lauta[0][0] = NULL;

			onkoValkeaKuningasLiikkunut = true;
			onkoValkeaDTliikkunut = true;

			valkeaKuningasRuutu = Ruutu(2, 0);
		}
		else if (siirtovuoro == 1)
		{ //musta
			lauta[2][7] = mk;
			lauta[3][7] = mt;
			lauta[4][7] = NULL;
			lauta[0][7] = NULL;

			onkoMustaKuningasLiikkunut = true;
			onkoMustaDTliikkunut = true;

			mustaKuningasRuutu = Ruutu(2, 7);
		}
	}

	// Kaksoisaskel
	Nappula *nappula = lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()];
	if (nappula != NULL && (nappula == vs || nappula == ms) &&
		abs(siirto->getLoppuruutu().getRivi() - siirto->getAlkuruutu().getRivi()) == 2)
	{
		kaksoisaskelSarakkeella = siirto->getLoppuruutu().getSarake();
	}
	else
	{
		kaksoisaskelSarakkeella = -1;
	}

	if (nappula != NULL && (nappula == vs || nappula == ms) && (siirto->getLoppuruutu().getRivi() == 0 || siirto->getLoppuruutu().getRivi() == 7))
	{

		switch (siirto->getMiksiKorotetaan())
		{
		case VD:
			lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] = vd;
			break;
		case VL:
			lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] = vl;
			break;
		case VT:
			lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] = vt;
			break;
		case VR:
			lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] = vr;
			break;
		case MD:
			lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] = md;
			break;
		case ML:
			lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] = ml;
			break;
		case MT:
			lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] = mt;
			break;
		case MR:
			lauta[siirto->getLoppuruutu().getSarake()][siirto->getLoppuruutu().getRivi()] = mr;
			break;
		}
	}

	// En passant
	if (siirto->onkoEnPassant())
	{
		lauta[siirto->getLoppuruutu().getSarake()][siirto->getAlkuruutu().getRivi()] = NULL;
	}

	siirtovuoro = !siirtovuoro;
	hash.tarkista(this);
}

bool Asema::getSiirtovuoro()
{
	return siirtovuoro;
}

void Asema::setSiirtovuoro(int vari)
{
	siirtovuoro = vari;
}

bool Asema::getOnkoValkeaKuningasLiikkunut()
{
	return onkoValkeaKuningasLiikkunut;
}

bool Asema::getOnkoMustaKuningasLiikkunut()
{
	return onkoMustaKuningasLiikkunut;
}

bool Asema::getOnkoValkeaDTliikkunut()
{
	return onkoValkeaDTliikkunut;
}

bool Asema::getOnkoValkeaKTliikkunut()
{
	return onkoValkeaKTliikkunut;
}

bool Asema::getOnkoMustaDTliikkunut()
{
	return onkoMustaDTliikkunut;
}

bool Asema::getOnkoMustaKTliikkunut()
{
	return onkoMustaKTliikkunut;
}

std::vector<Siirto> Asema::annaRaakaSiirrot()
{
	std::vector<Siirto> siirrot;
	siirrot.reserve(70);
	std::vector<Siirto> syonnit;
	syonnit.reserve(70);

	if (siirtovuoro == 0)
	{
		vDsiirrot = 0;
		vLsiirrot = 0;
		vTsiirrot = 0;
	}
	else
	{
		mDsiirrot = 0;
		mLsiirrot = 0;
		mTsiirrot = 0;
	}
	nappuloidenMaara = 0;
	for (size_t x = 0; x < 8; x++)
	{
		for (size_t y = 0; y < 8; y++)
		{
			Ruutu ruutu(x, y);
			Nappula *nappula = lauta[x][y];
			if (nappula != NULL)
			{
				nappuloidenMaara++;
				if (nappula->vari == siirtovuoro)
				{
					nappula->annaSiirrot(siirrot, syonnit, &ruutu, this, siirtovuoro);
				}
			}
		}
	}
	std::sort(syonnit.begin(), syonnit.end(), std::greater<Siirto>());
	//lista.sort(std::greater<Siirto>());
	syonnit.insert(syonnit.end(), siirrot.begin(), siirrot.end());
	return syonnit;
}

std::vector<Siirto> Asema::annaRaakaSiirrotKillerTt(Siirto ttSiirto, Siirto killer1, Siirto killer2)
{
	std::vector<Siirto> lista = annaRaakaSiirrot();
	std::stable_partition(lista.begin(), lista.end(), [ttSiirto, killer1, killer2](const Siirto &s) -> bool {
		if (s == ttSiirto)
			return true;
		if (s == killer1)
			return true;
		if (s == killer2)
			return true;
		return false;
	});
	return lista;
}

std::vector<Siirto> Asema::annaLaillisetSiirrot()
{
	std::vector<Siirto> lista = annaRaakaSiirrot();
	auto it = lista.begin();
	while (it != lista.end())
	{
		Siirto siirto = *it;
		Asema uusiAsema = Asema(*this);
		uusiAsema.paivitaAsema(&siirto);
		Ruutu kuningas = siirtovuoro == 0 ? uusiAsema.getValkeaKuningasRuutu() : uusiAsema.getMustaKuningasRuutu();
		if (uusiAsema.onkoRuutuUhattu(kuningas))
		{
			it = lista.erase(it);
		}
		else
		{
			++it;
		}
	}
	return lista;
}

bool Asema::onkoRuutuUhattu(Ruutu ruutu)
{
	std::vector<Siirto> lista = annaRaakaSiirrot();

	for (auto s : lista)
	{
		if (s.getLoppuruutu() == ruutu)
		{
			return true;
		}
	}
	return false;
}

Ruutu Asema::getValkeaKuningasRuutu()
{
	return valkeaKuningasRuutu;
}

Ruutu Asema::getMustaKuningasRuutu()
{
	return mustaKuningasRuutu;
}

Nappula *Asema::getNappulaRuudussa(int sarake, int rivi)
{
	if (sarake < 8 && sarake > -1 && rivi < 8 && rivi > -1)
	{
		return lauta[sarake][rivi];
	}
	return nullptr;
}

int Asema::laskeNappulatVieressa(int sarake, int rivi, int vari)
{
	int nappulat = 0;

	for (int x = -1; x <= 1; x++)
	{
		int xUusi = x + sarake;
		if (xUusi < 8 && xUusi >= 0)
		{
			for (int y = -1; y <= 1; y++)
			{
				int yUusi = y + rivi;
				if (yUusi < 8 && yUusi >= 0)
				{
					if (lauta[xUusi][yUusi] != NULL && lauta[xUusi][yUusi]->vari == vari)
					{
						nappulat++;
					}
				}
			}
		}
	}
	return nappulat;
}

bool Asema::onkoNappulaVieressa(Ruutu ruutu, int nappula)
{

	for (int x = -1; x <= 1; x++)
	{
		int xUusi = x + ruutu.getSarake();
		if (xUusi < 8 && xUusi >= 0)
		{
			for (int y = -1; y <= 1; y++)
			{
				int yUusi = y + ruutu.getRivi();
				if (yUusi < 8 && yUusi >= 0)
				{
					if (lauta[xUusi][yUusi] != NULL && lauta[xUusi][yUusi]->_koodi == nappula)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}
double Asema::tuplaSotilaat(int sarake, int rivi, int vari)
{
	double arvo = 0;
	Nappula *sama = vari ? ms : vs;
	for (size_t i = 0; i <= 8; i++)
	{
		if (i != rivi)
		{
			if (vari == 0 && lauta[sarake][i] == sama)
			{
				arvo -= 0.1;
			}
		}
	}
	return arvo;
}

bool Asema::onkoVapaaSotilas(int sarake, int rivi, int vari)
{
	int y = rivi;
	int x = sarake;
	int xMin = std::max(0, x - 1);
	int xMax = std::min(7, x + 1);
	if (vari == 0)
	{
		for (int i = y; i < 8; i++)
		{
			for (int j = xMin; j <= xMax; j++)
			{
				if (lauta[j][i] == ms)
				{
					return false;
				}
			}
		}
	}
	else
	{
		for (int i = y; i >= 0; i--)
		{
			for (int j = xMin; j <= xMax; j++)
			{
				if (lauta[j][i] == vs)
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool Asema::mahdotonLoppupeli()
{
	if (nappuloidenMaara > 4)
	{
		return false;
	}
	else if (nappuloidenMaara == 2)
	{
		return true;
	}
	else
	{
		int nappulatJaljella[12] = {0};
		int valkoisia = 0;
		int mustia = 0;
		for (size_t x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; ++y)
			{
				if (lauta[x][y] != nullptr)
				{
					nappulatJaljella[lauta[x][y]->_koodi]++;
					if (lauta[x][y]->vari == 0)
					{
						valkoisia++;
					}
					else
					{
						mustia++;
					}
				}
			}
		}
		if (mustia == 1) //vain kuningas j�ljell�
		{
			if (nappulatJaljella[VL] > 0 && valkoisia == 2)
				return true; //vain kuningas ja l�hetti
			if (nappulatJaljella[VR] > 0 && valkoisia == 2)
				return true; //vain kuningas ja ratsu
			if (nappulatJaljella[VR] == 2 && valkoisia == 3)
				return true; //vain kuningas ja 2 ratsua
		}
		else if (valkoisia == 1) //vain valkoinen kuningas j�ljell�
		{
			if (nappulatJaljella[ML] > 0 && mustia == 2)
				return true; //vain kuningas ja l�hetti
			if (nappulatJaljella[MR] > 0 && mustia == 2)
				return true; //vain kuningas ja ratsu
			if (nappulatJaljella[MR] == 2 && mustia == 3)
				return true; //vain kuningas ja 2 ratsua
		}
	}
	return false;
}

std::string Asema::getFen()
{
	std::string fen;
	for (int j = 7; j >= 0; j--)
	{
		int emptyInRow = 0;
		for (size_t i = 0; i < 8; i++)
		{
			if (lauta[i][j] != NULL && emptyInRow != 0)
			{
				fen += std::to_string(emptyInRow);
				emptyInRow = 0;
			}
			if (lauta[i][j] != NULL)
			{
				switch (lauta[i][j]->_koodi)
				{
				case VT:
					fen += "R";
					break;
				case VR:
					fen += "N";
					break;
				case VL:
					fen += "B";
					break;
				case VD:
					fen += "Q";
					break;
				case VK:
					fen += "K";
					break;
				case VS:
					fen += "P";
					break;
				case MT:
					fen += "r";
					break;
				case MR:
					fen += "n";
					break;
				case ML:
					fen += "b";
					break;
				case MD:
					fen += "q";
					break;
				case MK:
					fen += "k";
					break;
				case MS:
					fen += "p";
					break;
				}
			}
			else
			{
				emptyInRow++;
			}
		}
		if (emptyInRow != 0)
		{
			fen += std::to_string(emptyInRow);
		}
		fen += "/";
	}
	fen = fen.substr(0, fen.size() - 1);
	fen += " ";
	fen += (siirtovuoro == 0 ? "w" : "b");

	bool wKAllowed = !(getOnkoValkeaKuningasLiikkunut() || getOnkoValkeaKTliikkunut());
	bool wQAllowed = !(getOnkoValkeaKuningasLiikkunut() || getOnkoValkeaDTliikkunut());
	bool bKAllowed = !(getOnkoMustaKuningasLiikkunut() || getOnkoMustaKTliikkunut());
	bool bQAllowed = !(getOnkoMustaKuningasLiikkunut() || getOnkoMustaDTliikkunut());

	if (wKAllowed || wQAllowed || bKAllowed || bQAllowed)
	{
		fen += " ";
	}
	if (wKAllowed)
	{
		fen += "K";
	}
	if (wQAllowed)
	{
		fen += "Q";
	}
	if (bKAllowed)
	{
		fen += "k";
	}
	if (bQAllowed)
	{
		fen += "q";
	}

	return fen;
}

double Asema::quiescent(double alpha, double beta)
{
	double standing_pat = evaluoiNegamax();

	if (standing_pat >= beta)
	{
		return beta;
	}
	if (standing_pat > alpha)
	{
		alpha = standing_pat;
	}

	std::vector<Siirto> siirrot;
	siirrot.reserve(50);
	annaRaakaSiirrot();

	for (Siirto &s : siirrot)
	{
		if (!s.syonti && s.getMiksiKorotetaan() == 0)
		{
			continue;
		}
		stats.asemiaKokeiltu++;

		Asema uusiAsema = Asema(*this);
		uusiAsema.paivitaAsema(&s);

		double score = -uusiAsema.quiescent(-beta, -alpha);

		if (score >= beta)
		{
			return beta;
		}
		if (score > alpha)
		{
			alpha = score;
		}
	}
	return alpha;
}

MinMaxPaluu Asema::iteratiivinenAlphaBeta2(const lautaDict &historia, int kello, int *maxSyvyys)
{

	siirtokello.Aloita(kello);
	int depth = 1;
	MinMaxPaluu viimeParas;
	std::vector<MinMaxPaluu> viimePaluuLista;
	std::wcout << L"Syvyys: ";
	while (true)
	{
		std::wcout << depth << L" ";
		std::vector<MinMaxPaluu> paluuLista;
		MinMaxPaluu uusiParas = iterAlphaBetaRoot2(depth, &paluuLista, historia, viimeParas._parasSiirto);
		if (siirtokello.AikaLoppu())
		{
			break;
		}
		viimeParas = uusiParas;
		viimePaluuLista = paluuLista;
		depth++;

		if (depth > 30)
			break;
	}
	std::wcout << std::endl;

	//Printtaa siirrot
	int i = 0;
	for (auto s : viimePaluuLista)
	{
		std::wstring merkkiVali, linnaVali = L"";
		if ((siirtovuoro ? -s._evaluointiArvo : s._evaluointiArvo) >= 0)
		{
			merkkiVali = L" ";
		}
		if (s._parasSiirto.onkoLyhytLinna())
		{
			linnaVali = L"   ";
		}
		else if (s._parasSiirto.onkoPitkalinna())
		{
			linnaVali = L" ";
		}
		wprintf(L"|%s%s : %s%.2f ", linnaVali.c_str(), s._parasSiirto.toString(this).c_str(), merkkiVali.c_str(), (siirtovuoro ? -s._evaluointiArvo : s._evaluointiArvo));
		if (i++ == 5)
		{
			std::wcout << std::endl;
			i = 0;
		}
	}

	*maxSyvyys = depth;
	if (siirtovuoro)
	{
		viimeParas._evaluointiArvo = -viimeParas._evaluointiArvo;
	}

	return viimeParas;
}

MinMaxPaluu Asema::iterAlphaBetaRoot2(int syvyys, std::vector<MinMaxPaluu> *paluuarvot, lautaDict historia, Siirto viimeParasSiirto)
{
	double value = INT_MIN;
	double alphaNum = INT_MIN;
	double betaNum = INT_MAX;
	MinMaxPaluu paras;
	try
	{
		std::vector<Siirto> siirrot = annaLaillisetSiirrot();
		//Siirr� viime paras siirto ensimm�iseksi
		for (auto i = siirrot.begin(); i != siirrot.end(); ++i)
		{ //Etsi onko paras siirto listassa ja laita se listan eteen
			if (*i == viimeParasSiirto)
			{
				auto ps = *i;
				siirrot.erase(i);
				siirrot.emplace(siirrot.begin(), ps);
				//siirrot.emplace_front(ps);
				break;
			}
		}

		for (Siirto &s : siirrot)
		{
			Asema uusiAsema = Asema(*this);
			uusiAsema.paivitaAsema(&s);
			double nVal = -uusiAsema.iterAlphaBeta2(syvyys - 1, 0, historia, -betaNum, -alphaNum);
			MinMaxPaluu arvo;
			arvo._evaluointiArvo = nVal;
			arvo._parasSiirto = s;
			paluuarvot->push_back(arvo);

			value = std::max(value, nVal);
			if (value > alphaNum)
			{
				alphaNum = value;
				paras._evaluointiArvo = value;
				paras._parasSiirto = s;
			}
			if (alphaNum >= betaNum)
			{
				break;
			}
		}
	}
	catch (std::exception e)
	{
	}

	return paras;
}

double Asema::iterAlphaBeta2(int syvyys, int syvyysJuuresta, const lautaDict &historia, double alpha, double beta)
{
	if (siirtokello.AikaLoppu())
		throw std::exception();

	stats.asemiaKokeiltu++;
	if (tulossaTasapeli(historia) || mahdotonLoppupeli())
	{
		return 0;
	}
	else if (siirtovuoro == 0 && valkeaKuningasRuutu == Ruutu(-1, -1))
	{
		return -1000 - syvyys;
	}
	else if (siirtovuoro == 1 && mustaKuningasRuutu == Ruutu(-1, -1))
	{
		return -1000 - syvyys;
	}

	double alphaOrig = alpha;

	TranspositionItem *ttEntry = transposTable.retrieve(hash.hash);
	if (ttEntry != nullptr && ttEntry->syvyys >= syvyys)
	{
		stats.ttTauluOsumat++;
		if (ttEntry->solmuTyyppi == tarkka)
			return ttEntry->evaluaatio;
		else if (ttEntry->solmuTyyppi == ala)
			alpha = std::max(alpha, ttEntry->evaluaatio);
		else if (ttEntry->solmuTyyppi == yla)
			beta = std::min(beta, ttEntry->evaluaatio);
		if (alpha >= beta)
			return ttEntry->evaluaatio;
	}

	if (syvyys == 0)
	{
		return quiescent(alpha, beta);
	}

	Siirto ttSiirto;
	if (ttEntry == nullptr)
	{
		ttEntry = transposTable.retrieveOld(hash.hash);
		if (ttEntry != nullptr)
		{
			stats.ttTauluOsumat++;
		}
	}
	if (ttEntry != nullptr && ttEntry->syvyys > 1)
	{
		ttSiirto = ttEntry->parasSiirto;
	}
	Siirto killer1 = Siirto();
	bool killer1Found = GetKillerMove(&killer1, syvyysJuuresta, false);
	Siirto killer2 = Siirto();
	bool killer2Found = GetKillerMove(&killer2, syvyysJuuresta, true);

	std::vector<Siirto> siirrot = annaRaakaSiirrotKillerTt(ttSiirto, killer1, killer2);

	double value = INT_MIN;
	Siirto bestMove;
	for (Siirto &s : siirrot)
	{
		Asema uusiAsema(*this);
		uusiAsema.paivitaAsema(&s);
		double uusiValue = -uusiAsema.iterAlphaBeta2(syvyys - 1, syvyysJuuresta + 1, historia, -beta, -alpha);
		if (uusiValue > value)
		{
			value = uusiValue;
			bestMove = s;
		}
		alpha = std::max(alpha, value);
		if (alpha >= beta)
		{
			if (!s.syonti)
			{
				StoreKillerMove(s, syvyysJuuresta);
				if (s == killer1 || s == killer2)
				{
					stats.killerOikein++;
				}
			}
			break;
		}
	}

	if (ttSiirto == bestMove)
	{
		stats.ttParasOikein++;
	}

	if (value <= alphaOrig)
	{
		transposTable.store(hash.hash, bestMove, syvyys, value, yla);
	}
	else if (value >= beta)
	{
		transposTable.store(hash.hash, bestMove, syvyys, value, ala);
	}
	else
	{
		transposTable.store(hash.hash, bestMove, syvyys, value, tarkka);
	}

	return value;
}

bool Asema::tulossaTasapeli(const lautaDict &historia)
{
	Nappula **la = (Nappula **)lauta;
	auto found = historia.find(la);
	auto end = historia.end();
	if (found != end)
	{
		return found->second == 2;
	}
	return false;
}

static double sotilasRaaka[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 5, 5, 5, 5, 5, 5, 5,
	1, 1, 2, 3, 3, 2, 1, 1,
	5, 5, 1, 3, 3, 1, 5, 5,
	0, 0, 0, 2, 2, 0, 0, 0,
	5, -5, -1, 0, 0, -1, -5, 5,
	5, 1, 1, -2, -2, 1, 1, 5,
	0, 0, 0, 0, 0, 0, 0, 0};

static double ratsuRaaka[] = {
	-5, -4, -3, -3, -3, -3, -4, -5,
	-4, -2, 0, 0, 0, 0, -2, -4,
	-3, 0, 1, 1, 1, 1, 0, -3,
	-3, 5, 1, 2, 2, 1, 5, -3,
	-3, 0, 1, 2, 2, 1, 0, -3,
	-3, 5, 1, 1, 1, 1, 5, -3,
	-4, -2, 0, 5, 5, 0, -2, -4,
	-5, -4, -3, -3, -3, -3, -4, -5};

static double lahettiRaaka[] = {
	-2, -1, -1, -1, -1, -1, -1, -2,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 5, 1, 1, 5, 0, -1,
	-1, 5, 5, 1, 1, 5, 5, -1,
	-1, 0, 1, 1, 1, 1, 0, -1,
	-1, 1, 1, 1, 1, 1, 1, -1,
	-1, 5, 0, 0, 0, 0, 5, -1,
	-2, -1, -1, -1, -1, -1, -1, -2};

static double daamiRaaka[] = {
	-2, -1, -1, -5, -5, -1, -1, -2,
	-1, 0, 0, 0, 0, 0, 0, -1,
	-1, 0, 5, 5, 5, 5, 0, -1,
	-5, 0, 5, 5, 5, 5, 0, -5,
	0, 0, 5, 5, 5, 5, 0, -5,
	-1, 5, 5, 5, 5, 5, 0, -1,
	-1, 0, 5, 0, 0, 0, 0, -1,
	-2, -1, -1, -5, -5, -1, -1, -2};

static double torniRaaka[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 1, 1, 1, 1, 1, 1, 5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	0, 0, 5, 0, 0, 5, 0, 0};

static double kuningasKeskiRaaka[] = {
	-3, -4, -4, -5, -5, -4, -4, -3,
	-3, -4, -4, -5, -5, -4, -4, -3,
	-3, -4, -4, -5, -5, -4, -4, -3,
	-3, -4, -4, -5, -5, -4, -4, -3,
	-2, -3, -3, -4, -4, -3, -3, -2,
	-1, -2, -2, -2, -2, -2, -2, -1,
	2, 2, 0, 0, 0, 0, 2, 2,
	2, 3, 1, 0, 0, 1, 3, 2};

static double kuningasLoppuRaaka[] = {
	-5, -4, -3, -2, -2, -3, -4, -5,
	-3, -2, -1, 0, 0, -1, -2, -3,
	-3, -1, 2, 3, 3, 2, -1, -3,
	-3, -1, 3, 4, 4, 3, -1, -3,
	-3, -1, 3, 4, 4, 3, -1, -3,
	-3, -1, 2, 3, 3, 2, -1, -3,
	-3, -3, 0, 0, 0, 0, -3, -3,
	-5, -3, -3, -3, -3, -3, -3, -5};

static double eval_sotilaat[2][64];
static double eval_ratsut[2][64];
static double eval_lahetit[2][64];
static double eval_tornit[2][64];
static double eval_daamit[2][64];
static double eval_kuninkaat_keski[2][64];
static double eval_kuninkaat_loppu[2][64];

void alustaEvaluaatio(const double *raw, double dest[2][64])
{
	for (int i = 0; i < 64; i++)
	{
		dest[1][i] = raw[i] * -0.1;
		dest[0][i] = raw[63 - i] * 0.1;
	}
}

void Asema::AlustaEvaluaatio()
{
	alustaEvaluaatio(daamiRaaka, eval_daamit);
	alustaEvaluaatio(kuningasKeskiRaaka, eval_kuninkaat_keski);
	alustaEvaluaatio(kuningasLoppuRaaka, eval_kuninkaat_loppu);
	alustaEvaluaatio(lahettiRaaka, eval_lahetit);
	alustaEvaluaatio(ratsuRaaka, eval_ratsut);
	alustaEvaluaatio(sotilasRaaka, eval_sotilaat);
	alustaEvaluaatio(torniRaaka, eval_tornit);
}

/* 1. Laske nappuloiden arvo
Daami = 9
Torni = 5
L�hetti = 3,25
Ratsu = 3
Sotilas = 1

2. Kuninkaan hyvyys
Jos avaus tai keskipeli, niin hyv� ett� kunigas g1 tai b1/c1
Loppupeliss� vaikea sanoa halutaanko olla auttamassa omaa sotilasta korottumaan
vai olla est�m�ss� vastustajan korotusta siksi ei oteta kantaa
3. Arvosta keskustaa sotilailla ja ratsuilla
4. Arvosta pitki� linjoja daami, torni ja l�hetti
*/
double Asema::evaluoi()
{
	double laudanArvo = 0;
	//1. Nappuloiden arvo
	laudanArvo += 1.0 * paikkaEvaluointi();
	laudanArvo += 1 * sotilaidenLiikeArvo();
	//2. Kuningas turvassa
	laudanArvo += 1 * kuninkaanTurvallisuus();
	//3. Arvosta keskustaa
	laudanArvo += 1 * nappuloitaKeskella();
	// 4. Arvosta linjoja
	laudanArvo += 1 * linjat();
	return laudanArvo;
}

double Asema::evaluoiNegamax()
{
	if (siirtovuoro == 1)
		return -evaluoi();
	else
		return evaluoi();
}

double Asema::laskeNappuloidenArvo()
{
	double arvo = 0;
	int lahetit = 0;
	int lahetitM = 0;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (lauta[x][y] != NULL)
			{
				int a = lauta[x][y]->arvo;
				arvo += lauta[x][y]->arvo;
				if (a == 3.3)
				{
					lahetit++;
				}
				else if (a == 3.3)
				{
					lahetitM++;
				}
			}
		}
	}
	if (lahetit > 1)
	{
		arvo += 0.2;
	}
	if (lahetitM > 1)
	{
		arvo -= 0.2;
	}
	return arvo;
}

double Asema::paikkaEvaluointi()
{
	double arvo = 0;

	bool loppuPeli = nappuloidenMaara < 10;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (lauta[x][y] != NULL)
			{
				Nappula *nappula = lauta[x][y];
				arvo += nappula->arvo;
				int ruutu = (y * 8) + x;
				switch (nappula->_koodi)
				{
				case VS:
				case MS:
					arvo += eval_sotilaat[nappula->vari][ruutu];
					break;
				case VL:
					arvo += eval_lahetit[nappula->vari][ruutu];
					break;
				case ML:
					arvo += eval_lahetit[nappula->vari][ruutu];
					break;
				case VR:
				case MR:
					arvo += eval_ratsut[nappula->vari][ruutu];
					break;
				case VT:
				case MT:
					arvo += eval_tornit[nappula->vari][ruutu];
					break;
				case VK:
				case MK:
					if (loppuPeli)
					{
						arvo += eval_kuninkaat_loppu[nappula->vari][ruutu];
					}
					else
					{
						arvo += eval_kuninkaat_keski[nappula->vari][ruutu];
					}
					break;
				case VD:
				case MD:
					arvo += eval_daamit[nappula->vari][ruutu];
					break;
				}
			}
		}
	}
	return arvo;
}

double Asema::kuninkaanTurvallisuus()
{
	double arvo = 0;

	//l�hetti kuninkaan l�hell� +0.1
	if (onkoNappulaVieressa(valkeaKuningasRuutu, VL))
	{
		arvo += 0.1;
	}
	//ratsu kuninkaan l�hell� +0.1
	if (onkoNappulaVieressa(valkeaKuningasRuutu, VR))
	{
		arvo += 0.1;
	}

	if (onkoNappulaVieressa(mustaKuningasRuutu, ML))
	{
		arvo -= 0.1;
	}
	if (onkoNappulaVieressa(mustaKuningasRuutu, MR))
	{
		arvo -= 0.1;
	}

	//sotilaita kuninkaan edess� +0.05 per
	for (int i = -1; i <= 1; i++)
	{
		if (getNappulaRuudussa(valkeaKuningasRuutu.getSarake() + i, valkeaKuningasRuutu.getRivi() + 1) == vs)
		{
			arvo += 0.05;
		}
		if (getNappulaRuudussa(mustaKuningasRuutu.getSarake() + i, mustaKuningasRuutu.getRivi() - 1) == ms)
		{
			arvo -= 0.05;
		}
	}

	//linnoittautunut: +0.2
	if (valkeaKuningasRuutu.getSarake() == 6 && valkeaKuningasRuutu.getRivi() == 0)
	{
		if (getNappulaRuudussa(5, 0) == vt)
		{
			arvo += 0.2;
		}
	}
	else if (valkeaKuningasRuutu.getSarake() == 2 && valkeaKuningasRuutu.getRivi() == 0)
	{
		if (getNappulaRuudussa(3, 0) == vt)
		{
			arvo += 0.2;
		}
	}
	if (mustaKuningasRuutu.getSarake() == 6 && mustaKuningasRuutu.getRivi() == 7)
	{
		if (getNappulaRuudussa(5, 7) == mt)
		{
			arvo -= 0.2;
		}
	}
	else if (mustaKuningasRuutu.getSarake() == 2 && mustaKuningasRuutu.getRivi() == 7)
	{
		if (getNappulaRuudussa(3, 7) == mt)
		{
			arvo -= 0.2;
		}
	}

	return arvo;
}

double Asema::sotilaidenLiikeArvo()
{
	//Sotilas l�hemp�n� vastustajan laitaa +0.1 per askel 1
	//2 Sotilasta vierekk�in samalla rivill� -0.1 2
	//Sotilas blokattuna (ei p��se liikkumaan eteenp�in) -0.1 3
	//Sotilas yksin (ei ket��n omaa nappulaa l�hell�) -0.1 4
	//Jos toinen l�hetti sy�ty: sotilas samalla v�rill� kun elossa oleva l�hetti -0.1
	//L�hetti tai hevonen suoraan sotilaan takana +0.1 6

	double arvo = 0;

	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (lauta[x][y] == vs)
			{
				arvo += (y - 1) * 0.1; // 1
				if (getNappulaRuudussa(x + 1, y) == vs)
				{ // 2
					arvo -= 0.1;
				}
				if (getNappulaRuudussa(x, y + 1) != NULL)
				{ // 3
					arvo -= 0.1;
				}
				//if (laskeNappulatVieressa(x, y, 0) == 0) { // 4 Hidas, eik� vaikuta pelin tulokseen
				//	arvo -= 0.1;
				//}
				if (getNappulaRuudussa(x, y - 1) == vl || getNappulaRuudussa(x, y - 1) == vr)
				{ // 6
					arvo += 0.1;
				}
				if (onkoVapaaSotilas(x, y, 0))
				{ // 7
					arvo += 0.2;
				}
				arvo += tuplaSotilaat(x, y, 0);
			}
			else if (lauta[x][y] == ms)
			{
				arvo -= (6 - y) * 0.1;
				if (getNappulaRuudussa(x + 1, y) == ms)
				{
					arvo += 0.1;
				}
				if (getNappulaRuudussa(x, y - 1) != NULL)
				{
					arvo += 0.1;
				}
				/*if (laskeNappulatVieressa(x, y, 1) == 0) {
					arvo += 0.1;
				}*/
				if (getNappulaRuudussa(x, y + 1) == ml || getNappulaRuudussa(x, y + 1) == mr)
				{
					arvo -= 0.1;
				}
				if (onkoVapaaSotilas(x, y, 1))
				{ // 7
					arvo -= 0.2;
				}
				arvo -= tuplaSotilaat(x, y, 1);
			}
		}
	}

	return arvo;
}

double Asema::nappuloitaKeskella()
{
	double arvo = 0;

	//ratsut ydinkeskustassa + 0.1/napa
	if (lauta[3][3] == vr)
	{
		arvo += 0.1;
	}
	if (lauta[3][4] == vr)
	{
		arvo += 0.1;
	}
	if (lauta[4][3] == vr)
	{
		arvo += 0.1;
	}
	if (lauta[4][4] == vr)
	{
		arvo += 0.1;
	}
	if (lauta[3][3] == mr)
	{
		arvo -= 0.1;
	}
	if (lauta[3][4] == mr)
	{
		arvo -= 0.1;
	}
	if (lauta[4][3] == mr)
	{
		arvo -= 0.1;
	}
	if (lauta[4][4] == mr)
	{
		arvo -= 0.1;
	}
	//sotilaat laitakeskustassa + 0.05/napa
	//ratsut laitakeskustassa + 0.07/napa

	return arvo;
}

double Asema::linjat()
{
	//tornilla, l�hetill�, kuningattarella = +0.01 per mahdollinen liike
	return ((vDsiirrot + vTsiirrot + vLsiirrot) * 0.01) - ((mDsiirrot + mTsiirrot + mLsiirrot) * 0.01);

	//torni samalla rivill� kuin kuningatar = +0.05
}
