SRC_FILES_C := $(wildcard *.c)
FILES_C := $(patsubst %.c,%,$(SRC_FILES_C))
SRC_FILES_CPP := $(wildcard *.cc)
FILES_CPP := $(patsubst %.cpp,%,$(SRC_FILES_CPP))
CFLAGS := -ggdb3 -fno-omit-frame-pointer -std=c++20
OUT_DIR := out

all: $(FILES_C) $(FILES_CPP)

%: %.c _outdirs
	gcc $< $(CFLAGS) -o $@
	mv $@ $(OUT_DIR)/

%: %.cpp _outdirs
	g++ $< $(CFLAGS) -o $@
	mv $@ $(OUT_DIR)/

_outdirs:
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR)

.PHONY: all clean

