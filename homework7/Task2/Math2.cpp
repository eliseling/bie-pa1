// Wrapper to compile the C source as C++ in strict pedantic mode without the file-extension warning.
// This includes Math2.c into a .cpp translation unit so clang++/g++ won't warn about treating
// a .c input as C++ when invoked on this file.

#include "Math2.c"
