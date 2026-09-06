#include <emscripten/bind.h>
#include <string>
#include <vector>

#include "shakki/shakki/engine.h"

namespace {

std::string quote(const std::string& s) {
	std::string out = "\"";
	for (char c : s) {
		if (c == '"' || c == '\\') {
			out += '\\';
			out += c;
		}
		else if (c >= 0 && c < 0x20) {
			continue;
		}
		else {
			out += c;
		}
	}
	out += "\"";
	return out;
}

std::string quoteAll(const std::vector<std::string>& xs) {
	std::string out = "[";
	for (size_t i = 0; i < xs.size(); i++) {
		if (i) out += ",";
		out += quote(xs[i]);
	}
	out += "]";
	return out;
}

bool initDone = false;

void ensureInit() {
	if (!initDone) {
		shakki::init();
		initDone = true;
	}
}

std::string playTurn(std::string fen, int difficulty) {
	ensureInit();
	shakki::TurnResult r = shakki::playTurn(fen, difficulty);
	return "{\"fen\":" + quote(r.fen)
		+ ",\"lastMove\":" + quote(r.lastMove)
		+ ",\"enPassant\":" + quote(r.enPassant)
		+ ",\"moves\":" + quoteAll(r.legalMoves)
		+ "}";
}

std::string getLegalMoves(std::string fen) {
	ensureInit();
	return quoteAll(shakki::getLegalMoves(fen));
}

std::string getBestMove(std::string fen) {
	ensureInit();
	return shakki::getBestMove(fen);
}

std::string isGameOver(std::string fen) {
	ensureInit();
	return shakki::isGameOver(fen);
}

}

EMSCRIPTEN_BINDINGS(shakki) {
	emscripten::function("playTurn", &playTurn);
	emscripten::function("getLegalMoves", &getLegalMoves);
	emscripten::function("getBestMove", &getBestMove);
	emscripten::function("isGameOver", &isGameOver);
}
