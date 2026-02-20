#include "template.h"

#include <ctype.h>
#include <string.h>

static void strip_left(char** text)
{
    while (isspace(**text))
        *text += 1;
}

static void strip_right(char** text)
{
    size_t i = strlen(*text);
    for (; i-- > 0;)
        if (!isspace((*text)[i]))
            break;
    (*text)[i + 1] = '\0';
}

static void strip(char** text)
{
    strip_left(text);
    strip_right(text);
}

bool parse_template(Arena* arena, const char* text, const char* file_name, Template* out_template)
{
    bool success = true;

    const size_t text_len = strlen(text);

    // Will only be used in this function
    Arena temp_arena = { 0 };

    struct {
        struct {
            bool is_variable;
            char* items;
            size_t count;
            size_t capacity;
        }* items;
        size_t count;
        size_t capacity;
    } template_portions = { 0 };

    // We're using the temp_arena because the "template_portions" array is
    // temporary and won't be returned.
    arena_da_append(&temp_arena, &template_portions, (typeof(*template_portions.items)) { 0 });

    for (size_t i = 0; i < text_len; ++i) {
        if (text[i] == '\\' && !(i + 1 >= text_len)) {
            i++;
            arena_da_append(arena, &template_portions.items[template_portions.count - 1], text[i]);
            continue;
        }

        if (text[i] == '{') {
            arena_da_append(arena, &template_portions.items[template_portions.count - 1], '\0');
            // temp_arena usage!!
            arena_da_append(&temp_arena, &template_portions,
                            (typeof(*template_portions.items)) { .is_variable = true });
            continue;
        }

        if (template_portions.items[template_portions.count - 1].is_variable) {
            switch (text[i]) {
            case '}':
                arena_da_append(arena, &template_portions.items[template_portions.count - 1], '\0');
                // Modifying `.items` manually is not a good idea, but after
                // this call `arena_da_append` won't be called on this array
                // anyways so...
                strip(&template_portions.items[template_portions.count - 1].items);

                char* variable_name = template_portions.items[template_portions.count - 1].items;
                size_t variable_name_len = strlen(variable_name);
                for (size_t i = 0; i < variable_name_len; ++i) {
                    if (variable_name[i] == ' ') {
                        fprintf(stderr, "%s: ERROR: variables are not allowed to contain the character ` ` in their names\n",
                                file_name);
                        success = false;
                        goto defer;
                    }
                }

                // temp_arena usage!!
                arena_da_append(&temp_arena, &template_portions, (typeof(*template_portions.items)) { 0 });
                continue;
            case '"':
            case ':':
                fprintf(stderr, "%s: ERROR: variables are not allowed to contain the character `%c` in their names\n",
                        file_name, text[i]);
                success = false;
                goto defer;
            }
        }

        arena_da_append(arena, &template_portions.items[template_portions.count - 1], text[i]);
    }
    arena_da_append(arena, &template_portions.items[template_portions.count - 1], '\0');

    Template final_template = { 0 };
    {
        struct {
            TemplatePortion* items;
            size_t count;
            size_t capacity;
        } final_template_portions = { 0 };

        for (size_t i = 0; i < template_portions.count; ++i) {
            arena_da_append(arena, &final_template_portions, ((TemplatePortion) {
                                                                 .is_variable = template_portions.items[i].is_variable,
                                                                 .text = template_portions.items[i].items,
                                                             }));
        }

        final_template.portions = final_template_portions.items;
        final_template.portions_count = final_template_portions.count;
    }

    *out_template = final_template;

defer:
    arena_free(&temp_arena);
    return success;
}
