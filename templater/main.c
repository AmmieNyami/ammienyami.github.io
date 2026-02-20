#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include "input.h"
#include "template.h"

#define ARENA_IMPLEMENTATION
#include "external/arena.h"

static void usage(FILE* stream, const char* program_name)
{
    fprintf(stream, "%s - Generate a file from a template.\n", program_name);
    fprintf(stream, "USAGE: %s [options] <input_file> <output_file>\n", program_name);
    fprintf(stream, "    input_file:   File specifying which template it's gonna use and a value\n"
                    "                  for each template variable. Check the `--input-format` and\n"
                    "                  `--template-format` flags!\n"
                    "    output_file:  File that's going to be generated.\n"
                    "    options:\n"
                    "        --help             Prints this help and exits.\n"
                    "        --template-format  Prints help for the template format.\n"
                    "        --input-format     Prints help for the input format.\n");
}

static const char* shift(int* argc, const char*** argv)
{
    const char* arg = (*argv)[0];
    *argc -= 1;
    *argv += 1;
    return arg;
}

static bool read_entire_file(const char* file_path,
                             char** out_file_buf, size_t* out_file_size)
{
    *out_file_size = 0;
    *out_file_buf = NULL;

    bool result = true;

    FILE* f = fopen(file_path, "rb");
    if (!f) {
        fprintf(stderr, "ERROR: failed to open file `%s`: %s\n",
                file_path, strerror(errno));
        result = false;
        goto defer;
    }

    fseek(f, 0, SEEK_END);
    *out_file_size = ftell(f) + 1;
    fseek(f, 0, SEEK_SET);

    *out_file_buf = malloc(*out_file_size);

    if (fread(*out_file_buf, 1, *out_file_size - 1, f) != (size_t)*out_file_size - 1) {
        fprintf(stderr, "ERROR: failed to read from file file `%s`: %s\n",
                file_path, strerror(errno));
        result = false;
        goto defer;
    }

    (*out_file_buf)[*out_file_size - 1] = '\0';

defer:
    if (f)
        fclose(f);
    return result;
}

static void free_entire_file(char* file_buf, int file_size)
{
    (void)file_size;
    free(file_buf);
}

static bool input_and_template_from_file(Arena* arena, const char* file_path,
                                         Input* out_input, Template* out_template)
{
    Arena temp_arena = {0};

    char* input_file_source;
    size_t input_file_size;
    if (!read_entire_file(file_path, &input_file_source, &input_file_size))
        return false;

    if (!parse_input(arena, input_file_source, file_path, out_input))
        return false;

    struct {
        char* items;
        size_t count;
        size_t capacity;
    } input_directory = {0};
    arena_sb_append_cstr(&temp_arena, &input_directory, file_path);
    arena_sb_append_null(&temp_arena, &input_directory);
    input_directory.items = dirname(input_directory.items);

    struct {
        char* items;
        size_t count;
        size_t capacity;
    } template_file_path = {0};
    if (strncmp(out_input->template_path, "/", 1) == 0) {
        arena_sb_append_cstr(&temp_arena, &template_file_path, out_input->template_path);
        arena_sb_append_null(&temp_arena, &template_file_path);
    } else {
        arena_sb_append_cstr(&temp_arena, &template_file_path, input_directory.items);
        arena_sb_append_cstr(&temp_arena, &template_file_path, "/");
        arena_sb_append_cstr(&temp_arena, &template_file_path, out_input->template_path);
        arena_sb_append_null(&temp_arena, &template_file_path);
    }

    char* template_file_source;
    size_t template_file_size;
    if (!read_entire_file(template_file_path.items, &template_file_source, &template_file_size))
        return false;

    if (!parse_template(arena, template_file_source, template_file_path.items, out_template))
        return false;

    free_entire_file(template_file_source, template_file_size);
    free_entire_file(input_file_source, input_file_size);

    arena_free(&temp_arena);

    return true;
}

int main(int argc, const char** argv)
{
    const char* program_name = shift(&argc, &argv);

    if (argc < 1) {
        fprintf(stderr, "ERROR: input file not provided\n");
        usage(stderr, program_name);
        return 1;
    }

    const char* flag = shift(&argc, &argv);

    if (strcmp(flag, "--help") == 0) {
        usage(stdout, program_name);
        return 0;
    } else if (strcmp(flag, "--template-format") == 0) {
        fprintf(stdout, "The template format looks like this:\n"
                        "\n"
                        "```\n"
                        "Document created at {date}.\n"
                        "# {title}\n"
                        "The \\{content} is:\n"
                        "{content}\n"
                        "```\n"
                        "\n"
                        "Names between {brackets} are variables. Variables can contain any characters,\n"
                        "except for `:`, `\"` and ` `. There may be any amount of spaces between `{`\n"
                        "and the name of the variable. There may also be any amount of spaces between\n"
                        "the name of the variable and `}`.\n"
                        "\n"
                        "If the parser encounters a `\\`, it ignores it and appends the character right\n"
                        "after it, ignoring its meaning. For example, `\\{` ignores the `\\` and appends\n"
                        "`{`, ignoring its \"beginning of variable name\" meaning.\n"
                        "\n"
                        "%s replaces each {variable} with the curresponding value specified\n"
                        "by the file that uses this template (see `--input-format`).\n",
                program_name);
        return 0;
    } else if (strcmp(flag, "--input-format") == 0) {
        fprintf(stdout, "The input format looks like this:\n"
                        "\n"
                        "```\n"
                        "template \"./path/to/template.txt\"\n"
                        "\n"
                        "title: \"The Beginning\"\n"
                        "date: \"01/01/1970\"\n"
                        "-\n"
                        "\n"
                        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod\n"
                        "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim\n"
                        "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea\n"
                        "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate\n"
                        "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint\n"
                        "occaecat cupidatat non proident, sunt in culpa qui officia deserunt\n"
                        "mollit anim id est laborum.\n"
                        "```\n"
                        "\n"
                        "Each word, string and `:` may contain any amount of spaces between them.\n"
                        "\n"
                        "Every input must start with the keyword `template`. Following that, there\n"
                        "must be a string specifying a path to a template file (see `--template-format`).\n"
                        "After the template file path, there may optionally be a list of variables, followed\n"
                        "by a single `-`, regardless of there being a list of variables or not. Every\n"
                        "character after the first non-space character after the `-` will be interpreted\n"
                        "as part of a single string and will be automatically set as the value for the\n"
                        "variable `content`.\n"
                        "\n"
                        "The list of variables must have a sequence of variable definitions. A variable\n"
                        "definition must have a word, followed by a `:`, followed by a string. The word\n"
                        "will be the variable's name, and the string will be the variable's value.\n");
        return 0;
    }

    const char* input_file = flag;

    if (argc < 1) {
        fprintf(stderr, "ERROR: output file not provided\n");
        usage(stderr, program_name);
        return 1;
    }
    const char* output_file = shift(&argc, &argv);

    Arena arena = { 0 };

    Input input;
    Template template;
    if (!input_and_template_from_file(&arena, input_file, &input, &template)) {
        if (arena.begin != NULL && arena.end != NULL)
            arena_free(&arena);
        return 1;
    }

    FILE* file = fopen(output_file, "w");
    if (file == NULL) {
        fprintf(stderr, "ERROR: failed to open file `%s`: %s\n",
                output_file, strerror(errno));
        return 1;
    }

    for (size_t i = 0; i < template.portions_count; ++i) {
        if (template.portions[i].is_variable) {
            bool variable_found = false;
            for (size_t j = 0; j < input.variables_count; ++j) {
                if (strcmp(template.portions[i].text, input.variables[j].name) == 0) {
                    fprintf(file, "%s", input.variables[j].value);
                    variable_found = true;
                }
            }

            if (!variable_found) {
                fprintf(stderr, "ERROR: not all template variables were found in the input\n");
                arena_free(&arena);
                return 1;
            }
        } else {
            fprintf(file, "%s", template.portions[i].text);
        }
    }

    fclose(file);

    arena_free(&arena);

    return 0;
}
