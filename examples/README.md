## Examples

**Building**

_Dependency: `make`, `gcc`_

- `make all`
- `make clean`
- `make example-name-without-extension`

The resulting binaries are compiled in Debug mode and are placed inside
`examples/out/`.

-----------

**find-max-vas-size.c**

Тъй като размера на резервациите чрез `mmap` могат многократно да нахвърлят
действителния лимит на памет, на която можем да извършим _commit_,
да се намери възможно най-големия размер, за който всъщност е възможно.
