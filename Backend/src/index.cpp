#include <napi.h>
#include <string>
#include "test.h"
#include "HillClimb.h"
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include "shakki/peli.h"

Napi::String playChess(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	Hash::init();

	Peli(1);
	return Napi::String::New(env, "over");
}

Napi::String climber(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();
	std::string result = climb();

	return Napi::String::New(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
	exports.Set(
		// Napi::String::New(env, "greetHello"),
		// Napi::Function::New(env, greetHello));
		Napi::String::New(env, "playChess"),
		Napi::Function::New(env, playChess));

	return exports;
}

NODE_API_MODULE(greet, Init)