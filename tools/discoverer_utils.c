#include "discoverer_utils.h"

#include <string.h>
#include <stdbool.h>

#include <cgreen/internal/unit_implementation.h>

bool starts_with_cgreen_spec(const char *line) {
    char *cgreen_spec = CGREEN_SPEC_PREFIX CGREEN_SEPARATOR;
    return strncmp(line, cgreen_spec, strlen(cgreen_spec)) == 0 ? true : false;
}
