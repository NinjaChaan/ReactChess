# ReactChess

A chess app with React front end and a Node back end that serve a chess engine built with C++  compiled as a
native Node addon. You can play against the engine or against another person
(same network).

## What you need

- Node.js (18+), and pnpm or npm
- Python 3 and a C++ compiler — node-gyp uses these to build the engine
  - Linux: `build-essential` (Debian/Ubuntu) or `base-devel` (Arch)
  - Windows: Visual Studio Build Tools with the "Desktop development with C++" workload

## Setup

Install both parts. Installing the back end also compiles the engine, so make
sure the C++ toolchain is there first.

```bash
cd Backend && pnpm install
cd ../Frontend/react-chess && pnpm install
```

Build the front end (the back end serves it from `dist/`):

```bash
pnpm run build
```

## Run

```bash
cd Backend
pnpm run dev
```

Then open http://localhost:3000

## Playing on another device

The server listens on all interfaces, so open port 3000 in your firewall and
connect the other device to your machine's LAN IP (e.g.
`http://192.168.0.192:3000`) instead of localhost.

