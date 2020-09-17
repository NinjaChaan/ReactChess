#pragma once
#include <vector>
#include "asema.h"
#include "lautaDict.h"
#include "openingBook.h"
#include <iostream>
#include "kayttoliittyma.h"
#include "asema.h"
#include <list>
#include <chrono>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <napi.h>

class Bot : public Napi::ObjectWrap<Bot> {
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	//Bot();
	Bot(const Napi::CallbackInfo& info);
	std::tuple<std::string, std::string, std::vector<std::string>> Play_turn(std::string fen, int difficulty);
	std::vector<std::string> GetLegalMoves(std::string fen);
private:
	static Napi::FunctionReference constructor;
	Napi::FunctionReference jsFnRef;
    Napi::Function jsFn;

	Napi::Value playTurn(const Napi::CallbackInfo& info);
	Napi::Value getLegalMoves(const Napi::CallbackInfo& info);

	OpeningBook openingBook;
};