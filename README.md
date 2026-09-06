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
pnpm --dir Web install
pnpm --dir Web run build:all
pnpm --dir Web start
```

`build:all` bundles the front end and compiles the engine into `Web/dist/engine/`
(`shakki.js`, `shakki.wasm`, and `shakki.data`, which holds the opening book).
Everything needed to run the app ends up in `Web/dist/`. To rebuild only the
engine after changing C++, use `run build:wasm`.

`start` runs `server.js`, which serves `dist/` and relays multiplayer games. For
single-player alone, any static file server over `dist/` is enough.

The engine runs in a Web Worker, so the UI stays responsive while the bot
thinks. The opening book is consulted first and answers instantly; once out of
book the engine searches for about a second per move.

### Deploying

The engine artifacts in `dist/engine/` are committed, because no Node host has
Emscripten installed. Rebuild and commit them whenever the C++ changes;
everything else is built on the host.

**One service, single-player and multiplayer** — deploy `Web/` to a host that
runs a long-lived Node process (Render, Fly.io, Railway). Build with
`pnpm install && pnpm run build`, start with `node server.js`. `API_BASE` stays
empty because the relay serves the bundle from the same origin.

The relay keeps game state in memory and clients hold an open SSE connection, so
it must run as a **single always-on instance**. Request-scoped serverless
platforms (Vercel, Netlify Functions, Lambda) will not work, and neither will
multiple replicas.

**Static only** — for a host with no server at all:

```bash
pnpm --dir Web run build:demo
```

Same build with the human-vs-human UI hidden. Upload `dist/` to Netlify,
Cloudflare Pages, GitHub Pages, or similar. Prefer a host that serves from the
domain root; under a project subpath the engine assets still resolve (they are
located relative to the page), but it is one less thing to get wrong. `.wasm`
must be served as `application/wasm`, which those hosts do by default.

**Split** — static front end on one host, relay on another. Point the front end
at the relay at build time:

```bash
API_BASE=https://your-relay.example.com pnpm --dir Web run build:all
```

The relay allows any origin, so no further configuration is needed. On a LAN it
listens on all interfaces: open its port in your firewall and use your machine's
LAN IP (e.g. `http://192.168.0.192:3000`).

## Console

Needs a C++ compiler with C++20 support. Build from `Engine/src/shakki`, run
from `Engine/` — the game loads the opening book from `./db/*.tsv`:

```bash
make -C Engine/src/shakki shakki
cd Engine && ./bin/shakki
```

It asks how many human players (0, 1 or 2), then which colour you want, and
takes moves as coordinates.

## UCI engine

For plugging the engine into a chess GUI such as Cute Chess or Arena:

```bash
make -C Engine/src/shakki uci
cd Engine && ./bin/uci-shakki
```

## Notes on the engine

The transposition table is a pair of static arrays of `2^TT_SIZE_LOG2` entries
at 64 bytes each. The native builds default to `2^24`, which is 2 GiB of BSS and
needs `-mcmodel=large` on x86-64; the wasm build uses `2^18` (32 MiB) because
wasm32 cannot address that much. Override it per build:

```bash
make -C Engine/src/shakki TT_SIZE_LOG2=22 shakki
TT_SIZE_LOG2=20 pnpm --dir Web run build:wasm
```

This is the main strength-versus-memory lever.

The opening book (`Engine/db/*.tsv`) is indexed by position at startup: each
line is replayed from the starting position so lookups are stateless and
transpositions are found automatically. Roughly 2,250 positions load in about
150 ms.

## Layout

- `Engine/src/shakki/shakki/` — the engine. `engine.h`/`engine.cpp` is the
  interface used by the browser build; `peli.cpp` is the console game.
- `Engine/src/wasm_bindings.cpp` — Emscripten entry point.
- `Engine/src/shakki/UCIShakki/` — UCI front end.
- `Engine/build-wasm.sh` — the WebAssembly build.
- `Engine/db/*.tsv` — opening book.
- `Web/server.js` — serves the bundle and relays multiplayer games.
- `Web/src/engine/client.js` — runs the wasm engine in a Worker.

Chess piece images by [Cburnett](https://en.wikipedia.org/wiki/User:Cburnett),
[CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/).
