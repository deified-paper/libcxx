#ifndef BENCHMARK_COLORPRINT_H_
#define BENCHMARK_COLORPRINT_H_

#include <cstdarg>
#include <string>
#include <iostream>

namespace benchmark {
enum LogColor {
  COLOR_DEFAULT,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_YELLOW,
  COLOR_BLUE,
  COLOR_MAGENTA,
  COLOR_CYAN,
  COLOR_WHITE
};

std::string FormatString(const char* msg, va_list args);
std::string FormatString(const char* msg, ...);

void ColorPrintf(std::ostream& out, LogColor color, const char* fmt, ...);

}  // end namespace benchmark

#endif  // BENCHMARK_COLORPRINT_H_
