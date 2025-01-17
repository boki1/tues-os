SRC_FILES_C := $(wildcard *.c)
FILES_C := $(patsubst %.c,%,$(SRC_FILES_C))
SRC_FILES_CPP := $(wildcard *.cc)
FILES_CPP := $(patsubst %.cpp,%,$(SRC_FILES_CPP))
CFLAGS := -ggdb3 -fno-omit-frame-pointer -std=c++23
OUT_DIR := out

all: $(FILES_C) $(FILES_CPP)

atomic: atomic.cpp _outdirs
	nasm -g -f elf64 atomic.s
	g++ -Xlinker --no-demangle $(CFLAGS) atomic.cpp atomic.o -o out/atomic

lock: lock.cpp _outdirs
	nasm -g -f elf64 lock.s
	g++ -O0 -Xlinker --no-demangle $(CFLAGS) lock.cpp lock.o -o out/lock

rwlock: rwlock.cpp _outdirs
	g++ -O3 -std=c++23 rwlock.cpp -o out/rwlock

cachetest: cachetest.cpp
	g++ $< -O3 -o $@
	mv $@ $(OUT_DIR)/

cache_bm: cache_bm.cpp
	g++ $< -std=c++20 -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o cache_bm

%: %.c _outdirs
	gcc $< $(CFLAGS) -o $@
	mv $@ $(OUT_DIR)/

%: %.cpp _outdirs
	g++ $< $(CFLAGS) -o $@
	mv $@ $(OUT_DIR)/

_outdirs:
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR) *.o

.PHONY: all clean

