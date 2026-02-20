#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "external/arena.h"

typedef struct {
    const char* name;
    const char* value;
} InputVariable;

typedef struct {
    const char* template_path;

    InputVariable* variables;
    size_t variables_count;
} Input;

bool parse_input(Arena* arena, const char* text, const char* file_name, Input* out_input);
