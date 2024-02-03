# WIP

# b3.h

Header-only library for parsing [B3](https://www.b3.com.br/en_us/) FIX/SBE
messages.

To use it, just `#include` it as any other C header. Before the include
directory, ONLY ONCE, `#define B3_IMPLEMENTATION` to include the implementation
of the library.

## Compatibility

This is ISO C90 (`-pedantic -ansi`), but depends on extensions that GCC kindly
does not disable (`#pragma scalar_storage_order little-endian` and
`__attribute__((packed))`). Clang does fail to recognize the pragma and throws
warnings ignoring it, but you will probably be fine in a little endian machine.
