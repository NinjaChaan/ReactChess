#include "UCIEngine.h"
#include <sstream>
#include <chrono>
#include <math.h>

using namespace std;

UCIEngine::UCIEngine(std::queue<std::string>* outputQueue, std::mutex* queueLock) {
	_outputQueue = outputQueue;
	_queueLock = queueLock;
	_shouldSearch = false;
	_infiniteSearch = false;
	Hash::init();
	srand(time(NULL));
	Asema::AlustaEvaluaatio();
}

void UCIEngine::Run() {

	using clock = std::chrono::system_clock;
	using sec = std::chrono::duration<double>;

	std::thread([&] {
		while (true) {
			while (!_shouldSearch) {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			int depth = 1;
			MinMaxPaluu viimeParas;
			std::vector<MinMaxPaluu> viimePaluuLista;
			auto start = clock::now();

			Asema::stats.Reset();

			int vuoro = _asema.getSiirtovuoro();

			float time = 5;

			if (vuoro == 0 && _wtime > 0) {
				if (_movestogo > 0) {
					time = max(0.5f, (_wtime / 1000.0f / _movestogo) - 1);
				} else {
					time = max(0.5f, min(_wtime / 1000.0f / 20, 10.0f));
				}
			}
			if (vuoro == 1 && _btime > 0) {
				if (_movestogo > 0) {
					time = max(0.5f, (_btime / 1000.0f / _movestogo) - 1);
				} else {
					time = max(0.5f, min(_btime / 1000.0f / 20, 10.0f));
				}
			}

			if (_infiniteSearch) {
				_asema.siirtokello.Aloita(999999);
			} else if (_movetime > 0) {
				_asema.siirtokello.Aloita(_movetime / 1000.0f);
			} else{
				_asema.siirtokello.Aloita(time);
			}

			int numSameBest = 0;

			while (_shouldSearch) {

				/*_queueLock->lock();
				_outputQueue->push("info depth " + to_string(depth));
				_queueLock->unlock();*/

				std::vector<MinMaxPaluu> paluuLista;
				MinMaxPaluu uusiParas = _asema.iterAlphaBetaRoot2(depth, &paluuLista, _asemaHistoria, viimeParas._parasSiirto);

				if (_asema.siirtokello.AikaLoppu()) {
					_shouldSearch = false;
					break;
				}

				if (uusiParas._parasSiirto == viimeParas._parasSiirto) {
					numSameBest++;
				} else {
					numSameBest = 0;
				}

				const sec duration = clock::now() - start;

				stringstream ss;
				ss << "info score cp " << (int)(uusiParas._evaluointiArvo * 100);
				ss << " depth " << depth;
				ss << " time " << (int)(duration.count() * 1000);
				ss << " nodes " << Asema::stats.asemiaKokeiltu;
				ss << " pv " << uusiParas._parasSiirto.toUCIString(&_asema);
				
				int nodesPerSecond = (int)(Asema::stats.asemiaKokeiltu / duration.count());

				_queueLock->lock();
				_outputQueue->push(ss.str());
				_outputQueue->push("info nps " + to_string(nodesPerSecond));
				_queueLock->unlock();

				viimeParas = uusiParas;
				viimePaluuLista = paluuLista;

				if (!_infiniteSearch && numSameBest >= 6) {
					_shouldSearch = false;
				}

				if (_depth > 0 && depth >= _depth && !_infiniteSearch) {
					break;
				}

				depth++;

				if (depth > 30 && !_infiniteSearch)
					break;
			}

			_queueLock->lock();
			_outputQueue->push("bestmove " + viimeParas._parasSiirto.toUCIString(&_asema));
			_queueLock->unlock();
		}
	}).detach();
}

void UCIEngine::NewGame() {
}

void UCIEngine::SetPosition(std::string fen, std::vector<std::string> moves) {
	StopSearch();

	if (fen == "startpos") {
		_asema = Asema();
	} else {
		_asema = Asema(fen);
	}

	Asema::transposTable.uusiVuoro();

	_asemaHistoria = lautaDict();

	for (auto move : moves) {
		Siirto siirto = Siirto::fromUCIString(move, &_asema);
		_asema.paivitaAsema(&siirto);

		Nappula** lautaCopy = new Nappula * [64];
		Nappula** lauta = (Nappula**)_asema.lauta;
		memcpy(lautaCopy, lauta, sizeof(void*) * 64);

		auto found = _asemaHistoria.find(lautaCopy);
		if (found != _asemaHistoria.end()) {
			found->second = found->second + 1;
		} else {
			_asemaHistoria[lautaCopy] = 1;
		}
	}
}

void UCIEngine::StartSearch(bool ponder, int wtime, int btime,
	int winc, int binc, int movestogo, int depth, int nodes,
	int mate, int movetime, bool infinite, bool restrictSearch, vector<string> searchmoves) {

	_ponder = ponder;
	_wtime = wtime;
	_btime = btime;
	_winc = winc;
	_binc = binc;
	_movestogo = movestogo;
	_depth = depth;
	_nodes = nodes;
	_mate = mate;
	_movetime = movetime;
	_infiniteSearch = infinite;
	_restrictSearch = restrictSearch;
	_searchmoves = searchmoves;

	_shouldSearch = true;
}

void UCIEngine::StopSearch() {
	_shouldSearch = false;
	_infiniteSearch = false;
	_asema.siirtokello.Lopeta();
}
