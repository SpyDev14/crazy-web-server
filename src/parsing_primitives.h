/*
 * Module for parsing primitive types
*/

#pragma once

#include <stdlib.h>
#include "src/common.h"

[[nodiscard]] bool parse_n_u64(const char *str, size_t len, u64 *out);
[[nodiscard]] bool parse_u64(const char *str, u64 *out);

[[nodiscard]] bool parse_n_i64(const char *str, size_t len, i64 *out);
[[nodiscard]] bool parse_i64(const char *str, i64 *out);
