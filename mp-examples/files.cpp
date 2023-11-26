#include <thread>
#include <iostream>
#include <functional>
#include <atomic>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

uint32_t traverse_files(const fs::path &root) {
	std::atomic<uint32_t> count{0};
	std::function<void(const fs::path &p)> dfs;
	dfs = [&](const fs::path &p) {
		std::vector<std::thread> spawned;
		for (auto const& entry : fs::directory_iterator{p}) {
			if (entry.path() == "." || entry.path() == "..")
				continue;
			std::cout << entry.path() << '\n';
			if (entry.is_directory() && !entry.is_symlink())
				spawned.emplace_back(dfs, entry.path());
			++count;
		}
		for (auto &t : spawned)
			t.join();
	};

	std::thread root_thread(dfs, root);
	root_thread.join();
	return count;
}

int main(int argc, const char *argv[]) {
	const fs::path root = [&]() {
		if (argc < 2)
			return ".";
		return argv[1];
	}();

	const auto count = traverse_files(root);
	std::cout << "\n-----------------\nTotal entries found: " << count << '\n';

	return 0;
}
