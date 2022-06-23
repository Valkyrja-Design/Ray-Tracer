#ifndef IMAGE_H
#define IMAGE_H

// This is used to lower the compiler warning level so that the included header can compile cleanly
// because it won't compiler at normal warning level

// Disable pedantic warnings for this external library (stb_image.h)
#ifdef _MSC_VER
    // Microsoft Visual C++ Compiler
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

// Restore warning levels.
#ifdef _MSC_VER
    // Microsoft Visual C++ Compiler
    #pragma warning (pop)
#endif

#endif