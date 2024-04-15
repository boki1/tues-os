#include <cstdio>

#include "copy.h"

int main(int argc, char* argv[])
{
	if (argc < 3) {
		puts("Usage: fast-copy src dest");
		return 0;
	}

	return copy(argv[1], argv[2]);
}
