#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>

#include "giantarray.inc"
// #include "array.inc"

#undef QSORT_USE_STD
#undef QSORT_USE_SINGLETHREAD
#undef QSORT_USE_MULTITHREAD

#define QSORT_USE_STD

#ifdef QSORT_USE_STD

int main()
{
    qsort(arr.data(), arr.size(), sizeof(decltype(arr)::value_type), [](const void* x, const void* y) {
        const int arg1 = *static_cast<const int*>(x);
        const int arg2 = *static_cast<const int*>(y);
        return arg1 - arg2;
    });
    assert(std::is_sorted(std::cbegin(arr), std::cend(arr)));
    std::cout << "Done" << '\n';
    return 0;
}

#else

int main()
{

    return 0;
}

#endif
