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

%: %.c _outdirs
	gcc $< $(CFLAGS) -o $@
	mv $@ $(OUT_DIR)/

%: %.cpp _outdirs
	g++ $< $(CFLAGS) -o $@
	mv $@ $(OUT_DIR)/

_outdirs:
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR) atomic.o

.PHONY: all clean

