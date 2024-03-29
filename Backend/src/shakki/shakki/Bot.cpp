#include "Bot.h"

using namespace std;

Napi::FunctionReference Bot::constructor;

Napi::Object Bot::Init(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "Bot", {InstanceMethod("playTurn", &Bot::playTurn), InstanceMethod("getLegalMoves", &Bot::getLegalMoves), InstanceMethod("getBestMove", &Bot::getBestMove), InstanceMethod("isGameOver", &Bot::isGameOver)});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Bot", func);
	return exports;
};

Bot::Bot(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Bot>(info)
{
	Hash::init();
	srand(time(NULL));
	Asema::AlustaEvaluaatio();

	vector<string> bookFiles = {"./db/a.tsv", "./db/b.tsv", "./db/c.tsv", "./db/d.tsv", "./db/e.tsv"};
	openingBook.Initialize(bookFiles);
}

std::tuple<std::string, std::string, std::vector<std::string>, std::string> Bot::Play_turn(string fen, int difficulty)
{
	using clock = std::chrono::system_clock;
	using sec = std::chrono::duration<double>;
	std::tuple<std::string, std::string, std::vector<std::string>> returnValues;
	std::vector<std::string> laillisetSiirrot;

	lautaDict fake;
	Asema asema(fen);
	vector<Siirto> siirtolista = asema.annaLaillisetSiirrot();

	if (siirtolista.size() == 0)
	{
		wprintf(L"%s voitti!", asema.getSiirtovuoro() == 1 ? L"Valkoinen" : L"Musta");
		std::string fen = asema.getSiirtovuoro() == 1 ? "White wins" : "Black wins";
		return std::make_tuple(fen, "", std::vector<std::string>(), "-");
	}
	if (asema.mahdotonLoppupeli())
	{
		return std::make_tuple("Draw", "", std::vector<std::string>(), "-");
	}

	auto start = clock::now();

	int saavutettuSyvyys = 0;
	int maxSyvyys = 2;
	switch (difficulty)
	{
	case 1:
		maxSyvyys = 2;
		break;
	case 2:
		maxSyvyys = 4;
		break;
	case 3:
		maxSyvyys = 30;
		break;
	default:
		maxSyvyys = 2;
		break;
	}
	MinMaxPaluu paras;

	paras = asema.iteratiivinenAlphaBeta2(fake, 1, maxSyvyys, &saavutettuSyvyys);

	wcout << endl
		  << "Koneen suosittelema siirto: " << endl;
	wprintf(L"%s : %.2f\n", paras._parasSiirto.toString(&asema).c_str(), paras._evaluointiArvo);
	const sec duration = clock::now() - start;
	wprintf(L"Evaluointi kesti %.2fs. Syvyys: %d \n", duration.count(), saavutettuSyvyys - 1);

	std::vector<int> items = asema.transposTable.checkItems(saavutettuSyvyys);

	for (size_t i = 0; i < items.size(); i++)
	{
		wcout << "Depth " << i << ": " << items[i] << endl;
	}
	asema.stats.Print();
	asema.stats.Reset();
	std::string lastMove = paras._parasSiirto.toNormalString(&asema);
	asema.paivitaAsema(&paras._parasSiirto);

	for (auto s : asema.annaLaillisetSiirrot())
	{
		laillisetSiirrot.push_back(s.toNormalString(&asema));
	}
	if (laillisetSiirrot.size() == 0)
	{
		wprintf(L"%s voitti!", asema.getSiirtovuoro() == 1 ? L"Valkoinen" : L"Musta");
		std::string fen = asema.getSiirtovuoro() == 1 ? "White wins" : "Black wins";
		return std::make_tuple(fen, lastMove, std::vector<std::string>(), "-");
	}

	return std::make_tuple(asema.getFen(), lastMove, laillisetSiirrot, asema.getEnPassant());
}

Napi::Value Bot::playTurn(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	if (info.Length() < 1)
	{
		Napi::TypeError::New(env, "Wrong number of arguments")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	if (!info[0].IsString())
	{
		Napi::TypeError::New(env, "Pass a fen string to the function")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	if (!info[1].IsNumber())
	{
		Napi::TypeError::New(env, "Pass a difficulty number to the function")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	Napi::String fen = info[0].As<Napi::String>();
	Napi::Number difficulty = info[1].As<Napi::Number>();

	std::tuple<std::string, std::string, std::vector<std::string>, std::string> returnValues = Play_turn(fen.Utf8Value(), difficulty);

	Napi::Object returnArray = Napi::Object::New(env);

	uint32_t i = 0;

	returnArray["fen"] = Napi::String::New(info.Env(), std::get<0>(returnValues));
	returnArray["lastMove"] = Napi::String::New(info.Env(), std::get<1>(returnValues));

	for (size_t i = 0; i < std::get<2>(returnValues).size(); i++)
	{
		returnArray[i] = Napi::String::New(info.Env(), std::get<2>(returnValues)[i]);
	}
	returnArray["enPassant"] = Napi::String::New(info.Env(), std::get<3>(returnValues));

	return returnArray;
}

std::vector<std::string> Bot::GetLegalMoves(string fen)
{
	std::vector<std::string> laillisetSiirrot;

	lautaDict fake;
	Asema asema(fen);
	vector<Siirto> siirtolista = asema.annaLaillisetSiirrot();

	for (auto s : asema.annaLaillisetSiirrot())
	{
		laillisetSiirrot.push_back(s.toNormalString(&asema));
	}

	return laillisetSiirrot;
}

Napi::Value Bot::getLegalMoves(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	if (info.Length() < 1)
	{
		Napi::TypeError::New(env, "Wrong number of arguments")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	if (!info[0].IsString())
	{
		Napi::TypeError::New(env, "Pass a fen to the function")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	Napi::String fen = info[0].As<Napi::String>();

	std::vector<std::string> returnValues = GetLegalMoves(fen.Utf8Value());

	Napi::Object returnArray = Napi::Object::New(env);

	uint32_t i = 0;

	for (size_t i = 0; i < returnValues.size(); i++)
	{
		returnArray[i] = Napi::String::New(info.Env(), returnValues[i]);
	}

	return returnArray;
}

std::string Bot::GetBestMove(string fen)
{
	using clock = std::chrono::system_clock;
	using sec = std::chrono::duration<double>;
	std::vector<std::string> laillisetSiirrot;

	lautaDict fake;
	Asema asema(fen);
	vector<Siirto> siirtolista = asema.annaLaillisetSiirrot();

	auto start = clock::now();

	int saavutettuSyvyys = 0;
	int maxSyvyys = 30;

	MinMaxPaluu paras;

	paras = asema.iteratiivinenAlphaBeta2(fake, 1, maxSyvyys, &saavutettuSyvyys);

	wcout << endl
		  << "Koneen suosittelema siirto: " << endl;
	wprintf(L"%s : %.2f\n", paras._parasSiirto.toString(&asema).c_str(), paras._evaluointiArvo);
	const sec duration = clock::now() - start;
	wprintf(L"Evaluointi kesti %.2fs. Syvyys: %d \n", duration.count(), saavutettuSyvyys - 1);

	std::vector<int> items = asema.transposTable.checkItems(saavutettuSyvyys);

	for (size_t i = 0; i < items.size(); i++)
	{
		wcout << "Depth " << i << ": " << items[i] << endl;
	}
	asema.stats.Print();
	asema.stats.Reset();
	std::string lastMove = paras._parasSiirto.toNormalString(&asema);

	return lastMove;
}

Napi::Value Bot::getBestMove(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	if (info.Length() < 1)
	{
		Napi::TypeError::New(env, "Wrong number of arguments")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	if (!info[0].IsString())
	{
		Napi::TypeError::New(env, "Pass a fen to the function")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	Napi::String fen = info[0].As<Napi::String>();

	Napi::String returnValue = Napi::String::New(env, GetBestMove(fen.Utf8Value()));

	return returnValue;
}

std::string Bot::IsGameOver(string fen)
{
	Asema asema(fen);
	vector<Siirto> siirtolista = asema.annaLaillisetSiirrot();

	if (siirtolista.size() == 0)
	{
		// wprintf(L"%s voitti!", asema.getSiirtovuoro() == 1 ? L"Valkoinen" : L"Musta");
		return asema.getSiirtovuoro() == 1 ? "White wins" : "Black wins";
	}
	if (asema.mahdotonLoppupeli())
	{
		return "Draw";
	}
	return "Continue";
}

Napi::Value Bot::isGameOver(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	if (info.Length() < 1)
	{
		Napi::TypeError::New(env, "Wrong number of arguments")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	if (!info[0].IsString())
	{
		Napi::TypeError::New(env, "Pass a fen to the function")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	Napi::String fen = info[0].As<Napi::String>();

	Napi::String returnValue = Napi::String::New(env, IsGameOver(fen.Utf8Value()));

	return returnValue;
}