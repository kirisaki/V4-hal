/**
 * @file hal_error.cpp
 * @brief Error message string implementation for V4 HAL
 */

#include "v4/hal_error.h"

extern "C"
{
  const char* hal_strerror(int err)
  {
    switch (err)
    {
#define HAL_ERR(name, val, desc) \
  case val:                      \
    return desc;
#include "v4/hal_errors.def"
#undef HAL_ERR
      default:
        return "Unknown error";
    }
  }
}
