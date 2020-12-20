#include <iostream>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include "UCIEngine.h"
#include <sstream>

using namespace std;

std::mutex queueMutex;
std::queue<string> inputQueue;
std::queue<string> outputQueue;

vector<string> split(string text) {
	std::istringstream iss(text);
	std::vector<std::string> results(std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());
	return results;
}

int main() {

	bool debugMode = false;
	bool initialized = false;

	UCIEngine engine(&outputQueue, &queueMutex);

	std::thread([&] {
		while (true) {
			string cmd;
			getline(cin, cmd);

			const std::lock_guard<std::mutex> lock(queueMutex);
			inputQueue.push(cmd);
		}
	}).detach();

	while (true) {
		if (!inputQueue.empty()) {
			queueMutex.lock();
			string cmd = inputQueue.front();
			inputQueue.pop();
			queueMutex.unlock();

			auto cmdParts = split(cmd);

			if (cmdParts.size() == 0) continue;

			string cmd0 = cmdParts[0];

			if (cmd == "uci") {
				cout << "id name FoxBot" << endl;
				cout << "id author Foxi Foxtron" << endl;
				cout << "uciok" << endl;
			} else if (cmd == "debug on") {
				debugMode = true;
			} else if (cmd == "debug off") {
				debugMode = false;
			} else if (cmd == "isready") {
				if (!initialized) {
					engine.Run();
					initialized = true;
				}
				cout << "readyok" << endl;
			} else if (cmd0 == "setoption") {
				//set options
				//"setoption name Style value Risky\n"
				//"setoption name Clear Hash\n"
			} else if (cmd == "ucinewgame") {
				engine.NewGame();
			} else if (cmd0 == "position") {
				if (cmdParts[1] != "startpos" && cmdParts[1] != "fen") {
					if (debugMode)
						cout << "info string malformed position command" << endl;
					continue;
				}
				bool useFen = cmdParts[1] == "fen";

				string fen = useFen ? cmdParts[2] : "startpos";

				int movesPos = useFen ? 4 : 3;

				std::vector<std::string> moves;

				if (cmdParts.size() >= movesPos && cmdParts[movesPos - 1] != "moves") {
					if (debugMode)
						cout << "info string malformed position command" << endl;
					continue;
				}

				for (int i = movesPos; i < cmdParts.size(); i++) {
					moves.push_back(cmdParts[i]);
				}

				engine.SetPosition(fen, moves);
			} else if (cmd0 == "go") {
				bool ponder = false;
				int wtime = 0;
				int btime = 0;
				int winc = 0;
				int binc = 0;
				int movestogo = 0;
				int depth = 0;
				int nodes = 0;
				int mate = 0;
				int movetime = 0;
				bool infinite = false;
				bool restrictSearch = false;
				vector<string> searchmoves;
				for (int i = 1; i < cmdParts.size(); i++) {

					if (cmdParts[i] == "ponder") {
						ponder = true;
					} else if (cmdParts[i] == "wtime") {
						wtime = stoi(cmdParts[++i]);
					} else if (cmdParts[i] == "btime") {
						btime = stoi(cmdParts[++i]);
					} else if (cmdParts[i] == "winc") {
						winc = stoi(cmdParts[++i]);
					} else if (cmdParts[i] == "binc") {
						binc = stoi(cmdParts[++i]);
					} else if (cmdParts[i] == "movestogo") {
						movestogo = stoi(cmdParts[++i]);
					} else if (cmdParts[i] == "depth") {
						depth = stoi(cmdParts[++i]);
					} else if (cmdParts[i] == "nodes") {
						nodes = stoi(cmdParts[++i]);
					} else if (cmdParts[i] == "mate") {
						mate = stoi(cmdParts[++i]);
					} else if (cmdParts[i] == "movetime") {
						movetime = stoi(cmdParts[++i]);
					} else if (cmdParts[i] == "infinite") {
						infinite = true;
					} else if (cmdParts[i] == "searchmoves") {
						restrictSearch = true;
						i++;
						for (; i < cmdParts.size(); i++) {
							searchmoves.push_back(cmdParts[i]);
						}
					}
				}

				engine.StartSearch(ponder, wtime, btime, winc, binc, movestogo, depth, nodes, mate, movetime, infinite, restrictSearch, searchmoves);
			} else if (cmd == "stop") {
				engine.StopSearch();
			} else if (cmd == "ponderhit") {
				//Our ponder move was correct
			} else if (cmd == "quit") {
				break;
			}
		} else if (!outputQueue.empty()) {
			queueMutex.lock();
			string cmd = outputQueue.front();
			outputQueue.pop();
			queueMutex.unlock();

			cout << cmd << endl;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	return 0;
}
