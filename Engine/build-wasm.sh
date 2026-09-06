#!/usr/bin/env bash
# Builds the chess engine as a WebAssembly module for the browser.
# Requires emsdk on PATH (source ~/emsdk/emsdk_env.sh).
set -euo pipefail

cd "$(dirname "$0")"

OUT_DIR="${1:-../Web/dist/engine}"
mkdir -p "$OUT_DIR"

SRC=(
	src/wasm_bindings.cpp
	src/shakki/shakki/engine.cpp
	src/shakki/shakki/asema.cpp
	src/shakki/shakki/Siirto.cpp
	src/shakki/shakki/Ruutu.cpp
	src/shakki/shakki/nappula.cpp
	src/shakki/shakki/hashing.cpp
	src/shakki/shakki/transpositionTable.cpp
	src/shakki/shakki/stats.cpp
	src/shakki/shakki/siirtokello.cpp
	src/shakki/shakki/openingBook.cpp
)

em++ -O3 -std=gnu++20 \
	-DTT_SIZE_LOG2="${TT_SIZE_LOG2:-18}" \
	-D__forceinline=inline \
	-include climits -include cstring -include cstdlib -include cctype -include cstdint \
	-fwasm-exceptions \
	--bind \
	--preload-file db/a.tsv@/db/a.tsv \
	--preload-file db/b.tsv@/db/b.tsv \
	--preload-file db/c.tsv@/db/c.tsv \
	--preload-file db/d.tsv@/db/d.tsv \
	--preload-file db/e.tsv@/db/e.tsv \
	-sMODULARIZE=1 \
	-sEXPORT_NAME=createChessEngine \
	-sENVIRONMENT=worker,node \
	-sINITIAL_MEMORY=64MB \
	-sALLOW_MEMORY_GROWTH=1 \
	-sSTACK_SIZE=1MB \
	"${SRC[@]}" \
	-o "$OUT_DIR/shakki.js"

echo "built:"
ls -la "$OUT_DIR"
