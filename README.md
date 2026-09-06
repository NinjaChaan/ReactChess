# ReactChess

A chess engine written in C++, with two ways to play it:

- **Web** — the engine compiles to WebAssembly and runs in the browser, with a
  React front end. The single-player game is a fully static site with no server.
- **Console** — the engine as a native terminal program.

The engine also builds as a UCI engine for chess GUIs.

## Web

### What you need

- Node.js (18+), and pnpm or npm
- [Emscripten](https://emscripten.org/docs/getting_started/downloads.html):

  ```bash
  git clone https://github.com/emscripten-core/emsdk.git ~/emsdk
  cd ~/emsdk && ./emsdk install latest && ./emsdk activate latest
  source ~/emsdk/emsdk_env.sh
  ```

### Build and run

```bash
pnpm --dir Frontend/react-chess install
pnpm --dir Frontend/react-chess run build:all
npx serve Frontend/react-chess/dist
```

`build:all` bundles the front end and compiles the engine into
`Frontend/react-chess/dist/engine/` (`shakki.js`, `shakki.wasm`, and
`shakki.data`, which holds the opening book). Everything needed to run the app
ends up in `dist/`. To rebuild only the engine after changing C++, use
`run build:wasm`.

The engine runs in a Web Worker, so the UI stays responsive while the bot
thinks. The opening book is consulted first and answers instantly; once out of
book the engine searches for about a second per move.

### Deploying

```bash
pnpm --dir Frontend/react-chess run build:demo
```

Same build with the human-vs-human UI hidden, which is what you want for a
static host with no relay behind it. Upload `dist/` to Netlify, Cloudflare
Pages, GitHub Pages, or similar.

Prefer a host that serves from the domain root. Under a project subpath the
engine assets still resolve (they are located relative to the page), but it is
one less thing to get wrong. `.wasm` must be served as `application/wasm`; the
hosts above do this by default.

### Playing against another person

Multiplayer needs the relay in `Backend/`. It only passes moves between the two
browsers — the engine runs on each client — so it is plain JavaScript with no
native code and no build step.

```bash
pnpm --dir Backend install
pnpm --dir Backend start          # listens on $PORT, default 3000
```

Point the front end at it when building:

```bash
API_BASE=http://localhost:3000 pnpm --dir Frontend/react-chess run build:all
```

Leave `API_BASE` unset to use the same origin, which is what happens when the
relay serves `dist/` itself. Deploy the relay to any host that runs a long-lived
Node process — game state is in memory and clients hold an open SSE connection,
so request-scoped serverless platforms will not work, and it must run as a
single instance.

On a LAN, the relay listens on all interfaces: open its port in your firewall
and point `API_BASE` at your machine's LAN IP (e.g. `http://192.168.0.192:3000`).

## Console

Needs a C++ compiler with C++20 support. Build from `Backend/src/shakki`, run
from `Backend/` — the game loads the opening book from `./db/*.tsv`:

```bash
make -C Backend/src/shakki shakki
cd Backend && ./bin/shakki
```

It asks how many human players (0, 1 or 2), then which colour you want, and
takes moves as coordinates.

## UCI engine

For plugging the engine into a chess GUI such as Cute Chess or Arena:

```bash
make -C Backend/src/shakki uci
cd Backend && ./bin/uci-shakki
```

## Notes on the engine

The transposition table is a pair of static arrays of `2^TT_SIZE_LOG2` entries
at 64 bytes each. The native builds default to `2^24`, which is 2 GiB of BSS and
needs `-mcmodel=large` on x86-64; the wasm build uses `2^18` (32 MiB) because
wasm32 cannot address that much. Override it per build:

```bash
make -C Backend/src/shakki TT_SIZE_LOG2=22 shakki
TT_SIZE_LOG2=20 pnpm --dir Frontend/react-chess run build:wasm
```

This is the main strength-versus-memory lever.

The opening book (`Backend/db/*.tsv`) is indexed by position at startup: each
line is replayed from the starting position so lookups are stateless and
transpositions are found automatically. Roughly 2,250 positions load in about
150 ms.

## Layout

- `Backend/src/shakki/shakki/` — the engine. `engine.h`/`engine.cpp` is the
  interface used by the browser build; `peli.cpp` is the console game.
- `Backend/src/wasm_bindings.cpp` — Emscripten entry point.
- `Backend/src/shakki/UCIShakki/` — UCI front end.
- `Backend/build-wasm.sh` — the WebAssembly build.
- `Backend/db/*.tsv` — opening book.
- `Backend/server.js` — the multiplayer relay.
- `Frontend/react-chess/src/engine/client.js` — runs the wasm engine in a Worker.

Chess piece images by [Cburnett](https://en.wikipedia.org/wiki/User:Cburnett),
[CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/).
