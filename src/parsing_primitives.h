/*
 * Module for parsing primitive types
*/

#pragma once

#include <stdlib.h>
#include "src/common.h"

bool parse_n_u64(const char *str, size_t len, u64 *out);
bool parse_ulong(const char *str, u64 *out);

bool parse_n_i64(const char *str, size_t len, i64 *out);
bool parse_i64(const char *str, i64 *out);
