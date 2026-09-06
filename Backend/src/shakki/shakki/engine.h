#pragma once
#include <string>
#include <vector>

namespace shakki {

struct TurnResult {
	std::string fen;
	std::string lastMove;
	std::vector<std::string> legalMoves;
	std::string enPassant;
};

void init();

TurnResult playTurn(const std::string& fen, int difficulty);
std::vector<std::string> getLegalMoves(const std::string& fen);
std::string getBestMove(const std::string& fen);
std::string isGameOver(const std::string& fen);

}
