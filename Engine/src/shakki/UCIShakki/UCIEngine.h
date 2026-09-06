#pragma once
#include <vector>
#include "../shakki/asema.h"
#include "../shakki/lautaDict.h"
#include "../shakki/openingBook.h"
#include <iostream>
#include <list>
#include <chrono>
#include <algorithm>
#include <queue>
#include <mutex>

class UCIEngine {
private:
	std::queue<std::string>* _outputQueue;
	std::mutex* _queueLock;
	Asema _asema;
	lautaDict _asemaHistoria;
	bool _shouldSearch;
	bool _infiniteSearch;
	bool _ponder = false;
	int _wtime = 0;
	int _btime = 0;
	int _winc = 0;
	int _binc = 0;
	int _movestogo = 0;
	int _depth = 0;
	int _nodes = 0;
	int _mate = 0;
	int _movetime = 0;
	bool _restrictSearch = false;
	std::vector<std::string> _searchmoves;

public:
	UCIEngine(std::queue<std::string> *outputQueue, std::mutex *queueLock);
	void Run();
	void NewGame();
	void SetPosition(std::string fen, std::vector<std::string> moves);
	void StartSearch(bool ponder, int wtime, int btime,
		int winc, int binc, int movestogo, int depth, int nodes,
		int mate, int movetime, bool infinite, bool restrictSearch, std::vector<std::string> searchmoves);
	void StopSearch();
};