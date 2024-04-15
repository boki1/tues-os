#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <unistd.h>
#include <cerrno>

#define MUST(a) \
  { int b = (a); if(!b) { perror("err: "); res = errno; return -res; } }

#define CEIL_4KB(a) \
  ((a) + (1ull<<12) - 1 & -(1ull<<12))

#include "copy.h"

int copy2(const char* src_path, const char* dest_path, int* src_fd, int* dest_fd)
{
	int res = 0;
	struct stat statbuf{};
	uint8_t* dest_ptr;

	MUST((*src_fd = open(src_path, O_DIRECT | O_RDONLY)) > 0);
	MUST(!fstat(*src_fd, &statbuf));
	const size_t size = statbuf.st_size;
	const mode_t mode = statbuf.st_mode;

	MUST((*dest_fd = open(dest_path, O_CREAT | O_TRUNC | O_RDWR, mode)) > 0);
	MUST(!ftruncate(*dest_fd, size));

	MUST((dest_ptr = static_cast<uint8_t*>(mmap(nullptr, size, PROT_WRITE, MAP_SHARED, *dest_fd, 0))) != MAP_FAILED);
	MUST(read(*src_fd, dest_ptr, CEIL_4KB(size)));

	MUST(!munmap(dest_ptr, size));
	return 0;
}

int copy(const char* src_path, const char* dest_path)
{
	int src_fd, dest_fd;
	int res = copy2(src_path, dest_path, &src_fd, &dest_fd);
	if (res >= 0)
	{
		MUST(!close(src_fd));
		MUST(!close(dest_fd));
	}

	return res;
}
