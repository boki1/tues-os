## Examples

**Building**

_Dependency: `make`, `gcc`_

- `make all`
- `make clean`
- `make example-name-without-extension`

The resulting binaries are compiled in Debug mode and are placed inside
`examples/out/`.

**To do**

- [ ] Fix valloc in the "large" allocation case

-----------

**find-max-vas-size.c**

Тъй като размера на резервациите чрез `mmap` могат многократно да нахвърлят
действителния лимит на памет, на която можем да извършим _commit_,
да се намери възможно най-големия размер, за който всъщност е възможно.

-----------

**valloc.c**

Реализирайте алокатор, който да работи с цели страници и да не премества
памет при използваме на `realloc` операцията.
