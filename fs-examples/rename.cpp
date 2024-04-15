#include <cstdio>
#include <string>
#include <unistd.h>
#include <cstring>
#include <cassert>

#include "copy.h"

static inline void TRY(bool expr)
{
	if (!expr)
	{
		perror("error: ");
		abort();
	}
}

struct swapped
{
	explicit swapped(const std::string& orig)
		: m_orig{ orig }, m_swap{orig + ".swap"}
	{
		TRY(!copy2(m_orig.c_str(), m_swap.c_str(), &orig_fd, &swap_fd));
		TRY(orig_fd > -1 && !close(orig_fd));
		assert(swap_fd > -1);
	}

	~swapped() noexcept
	{
		TRY(!syncfs(swap_fd));
		TRY(!close(swap_fd));
		TRY(!rename(m_swap.c_str(), m_orig.c_str()));
	}

	template <typename T>
	void perform(T&& f)
	{
		f(swap_fd);
	}

	int swap_fd = -1, orig_fd = -1;
	const std::string m_swap;
	const std::string m_orig;
};

int main(int argc, char* argv[])
{
	const char *data = "This is some random data that is going to be put in the file, replacing its old content.\n";
	const size_t len = strlen(data) + 1;

	swapped{ argv[1] }.perform([&](const int fd)
	{
		TRY(!ftruncate(fd, len));
		TRY(write(fd, data, len) == len);
	});
}
