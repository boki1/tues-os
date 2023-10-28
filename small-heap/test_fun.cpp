#include "small_heap.h"

int main() {
    small_heap heap;
    auto big1 = heap.allocate(small_heap::PAGE_SIZE << 1);
    auto a1 = heap.allocate(13).value();
    auto a2 = heap.allocate(50).value();
    auto big2 = heap.allocate(small_heap::PAGE_SIZE << 1);


    // FIXME:
     auto a3 = heap.allocate(small_heap::PAGE_SIZE >> 2);

//  std::random_device rand{};
//  std::mt19937 generator(rand()); // mersenne_twister_engine seeded with rd()
//  std::uniform_int_distribution<> distrib(1, small_heap::BLOCK_SIZE - small_heap::PAGE_SIZE);
//
//  std::vector<small_heap::chunk> allocated;
//
//  auto make_allocations = [&, gen = 0]() mutable {
//    if (auto a = heap.allocate(distrib(generator)); a) {
//      std::memset(a->ptr, ++gen, a->size);
//      allocated.push_back(std::move(*a));
//    }
//  };
//
//  for (int i = 0; i < 10; ++i)
//    make_allocations();
//
//  std::uint8_t gen = 0;
//  for (auto &c : allocated) {
//    for (std::size_t i = 0; i < c.size; ++i)
//      assert(reinterpret_cast<std::uint8_t *>(c.ptr)[i] == gen);
//    ++gen;
//  }

    return 0;
}
