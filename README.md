# Simple embedded files for CMake projects

A simple and tiny tool for embedding files, written in C89.

### Why would I use this?

1. C23's `#embed` is cool and maybe will be supported by a small number of compilers in a decade or two;
2. `ld` magic is not available on Windows;
3. `xxd` is an external tool that you have to install before compiling, `semb` builds and runs from CMake.

### Why would I not use this?

1. CMake 3.20 is required for `cmake_path` command;
2. big files are slow to embed and even slower to compile;
3. having ~140 LOC dependency is a bit too much for you.

## Usage

1. Add this repo as a CMake subdirectory (you only need `semb.c` and `CMakeLists.txt`);
2. Use CMake function `semb_generate(my-target OUT output.h FILES docs/file1.txt ../res/img/file2.png)`;
   it will generate the following file:
   ```c
   static const unsigned char file1_txt[] = {
            0x00, 0x00, ...
   };
   static const unsigned char file2_png[] = {
            0x00, 0x00, ...
   };
   ```
3. Add `#include "output.h"` to some file from `my-target` and use embedded data however you like.

### Example

Let's make a program that prints its source code.

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
