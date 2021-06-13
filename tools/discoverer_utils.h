#ifndef DISCOVERER_UTILS_H
#define DISCOVERER_UTILS_H

#include <stdbool.h>
#include <stdio.h>

#ifdef UNITTESTING
int printf_unittesting(const char *fmt, ...);
#ifdef printf
#undef printf
#endif
#define printf printf_unittesting
#endif

bool starts_with_cgreen_spec(const char *line);

#endif
