### `mp-examples`, i.e multiprogramming examples

**Outline of topics covered**
- Multithreading with `std::thread`,
- Locking with `std::mutex`,
- Atomics with `std::atomic<>`.

----

_Example 0: `thread-count.cpp`_

Pretty useless program which demonstrates common data race problems when sharing data among multiple threads. Points out
tips regarding when to share work and when not to.

----

_Example 1: `files.cpp`_

Directory traversal using `std::filesystem` in which each directory is processed by its own thread.

----

_Example 2: `atomic.cpp & atomic.s`_

Implementation of an integer type which has only atomic operations using assembly.

----

_Example 3 `quicksort.cpp`_

Parallel quicksort implementation.
It uses the `array.inc` and `giantarray.inc` files which are generated using the following command (and 100 is changed
to a relatively appropriate _gigantic_ value):
```shell
$ FILE=test.inc
$ echo "std::array arr{" > ${FILE} \
  && seq -100 100 | shuf | xargs -I{} echo "{}, " >> ${FILE} \
  && echo "0 };" >> ${FILE} \
  && clang-format -i ${FILE};
```

----