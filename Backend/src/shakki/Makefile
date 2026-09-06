#   make shakki   console game, run from Backend/ (it reads ./db/*.tsv)
#   make uci      UCI engine for chess GUIs

CXX      ?= g++
CXXFLAGS := -O2 -std=gnu++20 -D__forceinline=inline \
            -include climits -include cstring -include cstdlib \
            -include cctype -include cstdint

# 2^24 entries needs -mcmodel=large to address 2 GiB of BSS.
TT_SIZE_LOG2 ?= 24
CXXFLAGS += -DTT_SIZE_LOG2=$(TT_SIZE_LOG2)
ifeq ($(shell uname -m),x86_64)
CXXFLAGS += -mcmodel=large
endif

BIN_DIR := ../../bin

CORE := shakki/asema.cpp shakki/Siirto.cpp shakki/Ruutu.cpp shakki/nappula.cpp \
        shakki/hashing.cpp shakki/transpositionTable.cpp shakki/stats.cpp \
        shakki/siirtokello.cpp shakki/openingBook.cpp

CONSOLE := $(CORE) shakki/main.cpp shakki/peli.cpp shakki/kayttoliittyma.cpp \
           shakki/vastustaja.cpp shakki/engine.cpp

UCI := $(CORE) shakki/engine.cpp UCIShakki/main.cpp UCIShakki/UCIEngine.cpp

.PHONY: all shakki uci clean
all: shakki

shakki: $(BIN_DIR)/shakki
uci: $(BIN_DIR)/uci-shakki

$(BIN_DIR)/shakki: $(CONSOLE)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I shakki $^ -o $@

$(BIN_DIR)/uci-shakki: $(UCI)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I shakki -pthread $^ -o $@

clean:
	rm -rf $(BIN_DIR)
