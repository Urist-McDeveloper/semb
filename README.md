# Embedded files for CMake projects

A simple tool for embedding files through static arrays, written in C89.

### Why would I use this?

1. C23's `#embed` is cool and may even be supported by a small number of compilers in a decade or two.
2. `ld` magic is not available on Windows.
3. `xxd` is an external tool that you have to install, `semb` builds and runs from CMake.

### Why would I not use this?

1. You don't need to embed files.
2. CMake 3.20 is required for `cmake_path` command.
3. Big files are slow to embed and even slower to compile.
4. Having ~140 LOC dependency is a bit too much for you.

## Usage

1. Add this repo as a CMake subdirectory (you only need `semb.c` and `CMakeLists.txt`);
2. Use CMake function `semb_generate`;
3. `#include` generated file and use embedded data.

##### The function

```cmake
semb_generate(<target> OUT <output.h> [BINARY] FILES <file1> [<fileN>...])
```

What it does:

* adds current binary directory as include directory for `target`;
* generates `output.h` in current binary directory during build process:
   * any relative path may be used, e.g. `../foo/bar.h`;
   * to `#include` the generated file simply use the same path;
* adds `output.h` to the source list of `target`.
* `fileN` are paths of files to be embedded:
   * if `BINARY` option is set, paths are resolved from current binary directory;
   * if `BINARY` option is not set, paths are resolved from current source directory.

##### A quick example

```cmake
semb_generate(my-target OUT embed.h FILES
        ../img/file1.png
        doc/file2.txt
        file3.ext)
```

Will generate:

```c
static const unsigned char file1_png[] = {
        0x00, 0x00, ...
};
static const unsigned char file2_txt[] = {
        0x00, 0x00, ...
};
static const unsigned char file3_ext[] = {
        0x00, 0x00, ...
};
   ```

### Full example

Let's make a program that prints its own source code.

##### Project structure

```
.
├─ external
│  └─ semb
│     ├─ CMakeLists.txt
│     └─ semb.c
├─ CMakeLists.txt
└─ main.c   
```

##### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(example C)

add_subdirectory(external/semb)

add_executable(example main.c)
semb_generate(example OUT embed_main.h FILES main.c)
```

##### main.c

```c
#include <stdio.h>
#include "embed_main.h"

int main() {
    for (int i = 0; i < sizeof(main_c); i++) {
        printf("%c", (char)main_c[i]);
    }
    return 0;
}
```
