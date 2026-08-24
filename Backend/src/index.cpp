#include <napi.h>
#include <string>
#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#endif
#include <iostream>
#include <string>
#include "shakki/shakki/Bot.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Bot::Init(env, exports);
    return exports;
}

NODE_API_MODULE(greet, Init)