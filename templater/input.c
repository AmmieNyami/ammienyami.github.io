#include "input.h"

#include "external/arena.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    const char* file_name;

    const char* text;
    size_t text_len;

    size_t cursor;
} InputParser;

static bool input_parser_check_eof(InputParser* parser)
{
    return parser->cursor >= parser->text_len;
}

static void input_parser_strip_whitespace(InputParser* parser)
{
    while (!input_parser_check_eof(parser) && isspace(parser->text[parser->cursor]))
        parser->cursor += 1;
}

// Returns NULL on error.
static const char* input_parser_parse_word(InputParser* parser, Arena* arena)
{
    if (parser->text[parser->cursor] == '"' || parser->text[parser->cursor] == ':') {
        fprintf(stderr, "%s: ERROR: unexpected `%c`\n",
                parser->file_name, parser->text[parser->cursor]);
        return NULL;
    }

    struct {
        char* items;
        size_t count;
        size_t capacity;
    } word = { 0 };

    while (!input_parser_check_eof(parser) && !isspace(parser->text[parser->cursor])
           && parser->text[parser->cursor] != ':' && parser->text[parser->cursor] != '"') {
        arena_da_append(arena, &word, parser->text[parser->cursor]);
        parser->cursor += 1;
    }

    arena_da_append(arena, &word, '\0');

    return word.items;
}

// Returns NULL on error.
static const char* input_parser_parse_string(InputParser* parser, Arena* arena)
{
    if (parser->text[parser->cursor] != '"') {
        fprintf(stderr, "%s: ERROR: expected beginning of string (`\"`) but got `%c`\n",
                parser->file_name, parser->text[parser->cursor]);
        return NULL;
    }
    parser->cursor++;

    struct {
        char* items;
        size_t count;
        size_t capacity;
    } string = { 0 };

    while (!input_parser_check_eof(parser) && parser->text[parser->cursor] != '"') {
        if (parser->text[parser->cursor] == '\\'
            && !(parser->cursor + 1 >= parser->text_len) && parser->text[parser->cursor + 1] == '"') {
            parser->cursor++;
            arena_da_append(arena, &string, parser->text[parser->cursor]);
        } else {
            arena_da_append(arena, &string, parser->text[parser->cursor]);
        }
        parser->cursor++;
    }

    if (parser->text[parser->cursor] != '"') {
        fprintf(stderr, "%s: ERROR: unclosed string\n",
                parser->file_name);
        return NULL;
    }
    parser->cursor++;

    arena_da_append(arena, &string, '\0');

    return string.items;
}

static bool input_parser_parse(InputParser* parser, Arena* arena, Input* out_input)
{
    bool success = true;

    // Will only be used by this functions. Gets freed at the end of the
    // scope.
    Arena temp_arena = { 0 };

    /*
     * Parse the `template` keyword
     */

    input_parser_strip_whitespace(parser);
    if (input_parser_check_eof(parser)) {
        fprintf(stderr, "%s: ERROR: expected keyword `template` but got nothing\n",
                parser->file_name);
        success = false;
        goto defer;
    }

    const char* template_keyword = input_parser_parse_word(parser, &temp_arena);
    if (template_keyword == NULL) {
        success = false;
        goto defer;
    }
    if (strcmp(template_keyword, "template") != 0) {
        fprintf(stderr, "%s: ERROR: expected keyword `template` but got `%s`\n",
                parser->file_name, template_keyword);
        success = false;
        goto defer;
    }

    /*
     * Parse the template path
     */

    input_parser_strip_whitespace(parser);
    if (input_parser_check_eof(parser)) {
        fprintf(stderr, "%s: ERROR: expected string but got nothing\n",
                parser->file_name);
        success = false;
        goto defer;
    }

    // `template_path` corresponds to `Input.template_path`, and therefore
    // this pointer must be kept after this function retruns. Because of that
    // we'll be using the arena provided for the current function.
    const char* template_path = input_parser_parse_string(parser, arena);
    if (template_path == NULL) {
        success = false;
        goto defer;
    }

    /*
     * Parse the variables
     */

    struct {
        InputVariable* items;
        size_t count;
        size_t capacity;
    } input_variables = { 0 };

    input_parser_strip_whitespace(parser);
    while (!input_parser_check_eof(parser) && parser->text[parser->cursor] != '-') {
        // Variable names and values are also returned, so we're also using
        // the provided arena here instead of the temporary one.
        const char* variable_name = input_parser_parse_word(parser, arena);
        if (variable_name == NULL) {
            success = false;
            goto defer;
        }

        // Check if variable already exists
        for (size_t i = 0; i < input_variables.count; ++i) {
            if (strcmp(variable_name, input_variables.items[i].name) == 0) {
                fprintf(stderr, "%s: ERROR: variable `%s` already exists\n",
                        parser->file_name, variable_name);
                success = false;
                goto defer;
            }
        }
        if (strcmp(variable_name, "content") == 0) {
            fprintf(stderr, "%s: ERROR: variable `%s` already exists\n",
                    parser->file_name, variable_name);
            success = false;
            goto defer;
        }

        input_parser_strip_whitespace(parser);
        if (input_parser_check_eof(parser)) {
            fprintf(stderr, "%s: ERROR: expected `:` but got nothing\n",
                    parser->file_name);
            success = false;
            goto defer;
        }

        if (parser->text[parser->cursor] != ':') {
            fprintf(stderr, "%s: ERROR: expected `:` but got `%c`\n",
                    parser->file_name, parser->text[parser->cursor]);
            success = false;
            goto defer;
        }

        parser->cursor++;

        input_parser_strip_whitespace(parser);
        if (input_parser_check_eof(parser)) {
            fprintf(stderr, "%s: ERROR: expected string but got nothing\n",
                    parser->file_name);
            success = false;
            goto defer;
        }

        const char* variable_value = input_parser_parse_string(parser, arena);
        if (variable_value == NULL) {
            success = false;
            goto defer;
        }
        input_parser_strip_whitespace(parser);

        arena_da_append(arena, &input_variables, ((InputVariable) {
                                                     .name = variable_name,
                                                     .value = variable_value,
                                                 }));
    }

    if (parser->text[parser->cursor] != '-') {
        fprintf(stderr, "%s: ERROR: expected '-' at the end of the variable definitions\n",
                parser->file_name);
        success = false;
        goto defer;
    }
    parser->cursor += 1;

    input_parser_strip_whitespace(parser);

    struct {
        char* items;
        size_t count;
        size_t capacity;
    } input_content = { 0 };

    while (!input_parser_check_eof(parser)) {
        arena_da_append(arena, &input_content, parser->text[parser->cursor]);
        parser->cursor += 1;
    }
    arena_da_append(arena, &input_content, '\0');

    arena_da_append(arena, &input_variables, ((InputVariable) {
                                                 .name = "content",
                                                 .value = input_content.items,
                                             }));

    Input final_input = {
        .template_path = template_path,
        .variables = input_variables.items,
        .variables_count = input_variables.count,
    };

    *out_input = final_input;

defer:
    if (temp_arena.begin != NULL && temp_arena.end != NULL)
        arena_free(&temp_arena);
    return success;
}

bool parse_input(Arena* arena, const char* text, const char* file_name, Input* out_input)
{
    InputParser parser = { .text = text, .text_len = strlen(text), .file_name = file_name };
    return input_parser_parse(&parser, arena, out_input);
}
