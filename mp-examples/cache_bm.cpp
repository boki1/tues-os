#include <cstdint>

#include <benchmark/benchmark.h>

static void BM_IncManyInts(benchmark::State& state) {
  const std::uint64_t size = state.range(0);
  std::uint64_t *arr = new std::uint64_t[size];
  for (auto _ : state) {
	  for (std::uint64_t i = 0; i < size; ++i)
		  ++arr[i];
  }
  delete[] arr;
}
BENCHMARK(BM_IncManyInts)->DenseRange(1 << 9, 1 << 15, 1 << 8);

BENCHMARK_MAIN();
