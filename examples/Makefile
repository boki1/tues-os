SRC_FILES := $(wildcard *.c)
FILES := $(patsubst %.c,%,$(SRC_FILES))
CFLAGS := -ggdb3 -fno-omit-frame-pointer
OUT_DIR := out

all: $(FILES)

%: %.c _outdirs
	gcc $< $(CFLAGS) -o $@
	mv $@ $(OUT_DIR)/

_outdirs:
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR)

.PHONY: all clean

