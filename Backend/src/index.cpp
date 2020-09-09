#include <napi.h>
#include <string>
#include "test.h"
#include "HillClimb.h"
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include "shakki/shakki/Bot.h"

//Bot* b = new Bot();

//  Napi::Function initializeBot(const Napi::CallbackInfo &info)
// {
// 	Bot b = Bot();
// 	Napi::Env env = info.Env();
// 	return Bot();
// }

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Bot::Init(env, exports);
    return exports;
}

// Napi::String playTurn(const Napi::CallbackInfo &info, std::string fen)
// {
// 	Napi::Env env = info.Env();
// 	return Napi::String::New(env, b->Play_turn(fen));
// }

Napi::String climber(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	std::string result = climb();

	return Napi::String::New(env, result);
}

// Napi::Object Init(Napi::Env env, Napi::Object exports)
// {
// 	exports.Set(
// 		Napi::String::New(env, "initializeBot"),
// 		Napi::Function::New(env, initializeBot));
// 	// exports.Set(
// 	// 	Napi::String::New(env, "playTurn"),
// 	// 	Napi::Function::New(env, playTurn));

// 	return exports;
// }

NODE_API_MODULE(greet, Init)