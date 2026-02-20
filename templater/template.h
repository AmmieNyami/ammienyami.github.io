#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "external/arena.h"

typedef struct {
    bool is_variable;
    const char* text;
} TemplatePortion;

typedef struct {
    TemplatePortion* portions;
    size_t portions_count;
} Template;

bool parse_template(Arena* arena, const char* text, const char* file_name, Template* out_template);
