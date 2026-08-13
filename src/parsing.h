/*
 * Module for parsing base types
*/

#pragma once

#include <stdlib.h>
#include "src/common.h"

// variant for not null-terminated str
bool parse_n_long(const char *str, size_t len, long *out);
bool parse_long(const char *str, long *out);
