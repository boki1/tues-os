### Условие

В резервирано виртуално адресно пространство от 2ТВ реализирайте алокатор,
който поддържа единствено малки алокации. Адресното пространство се разглежда
на 2МВ блокове, всеки от които е разделен на отделени 4KB-страници, в които
се съдържат 64В парчета, от които се раздава при извършване на заделяща
операция. Първата страница на всеки блок е побитова карта
(_bitmap_), която обозначава точно кои от малките парченца по 64B са
заети и кои - свободни. Целта в реализицията на този алокатор е да се
извършва _commit_ само тези цели 4K страници, в които има заети парчета
по 64B, и съответно - _decommit_ на онези, в които вече няма заети парчета.
Поради структурата, чрез която се запазват метаданните, най-голямото
възможно заделяне е 2MB - 4KB.

## Building

```
$ cmake -S. -Bcmake-build-debug -GNinja
$ cmake --build cmake-build-debug
$ cd cmake-build-debug && ctest --rerun-failed --output-on-failure
```

## To do

- [ ] Test more of the allocation
- [ ] Implement deallocation
