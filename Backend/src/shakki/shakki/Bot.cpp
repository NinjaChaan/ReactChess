#include "Bot.h"

using namespace std;

Napi::FunctionReference Bot::constructor;

Napi::Object Bot::Init(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "Bot", {InstanceMethod("playTurn", &Bot::playTurn)});

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

std::string Bot::Play_turn(string fen)
{

	using clock = std::chrono::system_clock;
	using sec = std::chrono::duration<double>;

	lautaDict fake;
	Asema asema(fen);
	vector<Siirto> siirtolista = asema.annaLaillisetSiirrot();

	if (siirtolista.size() == 0)
	{
		wprintf(L"%s voitti!", asema.getSiirtovuoro() == 1 ? L"Valkoinen" : L"Musta");
		return asema.getSiirtovuoro() == 1 ? "White wins" : "Black wins";
	}

	auto start = clock::now();

	int maxSyvyys = 0;
	MinMaxPaluu paras;

	paras = asema.iteratiivinenAlphaBeta2(fake, 5, &maxSyvyys);

	wcout << endl
		  << "Koneen suosittelema siirto: " << endl;
	wprintf(L"%s : %.2f\n", paras._parasSiirto.toString(&asema).c_str(), paras._evaluointiArvo);
	const sec duration = clock::now() - start;
	wprintf(L"Evaluointi kesti %.2fs. Syvyys: %d \n", duration.count(), maxSyvyys - 1);

	std::vector<int> items = asema.transposTable.checkItems(maxSyvyys);

	for (size_t i = 0; i < items.size(); i++)
	{
		wcout << "Depth " << i << ": " << items[i] << endl;
	}
	asema.stats.Print();
	asema.stats.Reset();

	asema.paivitaAsema(&paras._parasSiirto);

	return asema.getFen();
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
		Napi::TypeError::New(env, "Pass a fen to the function")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	Napi::String fen = info[0].As<Napi::String>();

	string newFen = Play_turn(fen.Utf8Value());

	return Napi::String::New(env, newFen);
}