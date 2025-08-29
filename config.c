/*
 * @encode: utf-8
 * @Date: 2025-08-24 16:53:51
 * @LastEditTime: 2025-08-26 17:54:39
 * @FilePath: /copy paste optimize/src/config.c
 */
#include <stdio.h>
#include <yaml.h>
#include <ctype.h>
#include <string.h>
#include "config.h"

int str_casecmp(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        char c1 = tolower((unsigned char)*s1);
        char c2 = tolower((unsigned char)*s2);
        if (c1 != c2)
        {
            return (unsigned char)c1 - (unsigned char)c2;
        }
        s1++;
        s2++;
    }
    return (unsigned char)tolower((unsigned char)*s1) -
           (unsigned char)tolower((unsigned char)*s2);
}

bool get_bool_config(const char *filePath, const char *key)
{
    FILE *f = fopen(filePath, "r");
    yaml_parser_t parser;
    yaml_event_t event;
    bool found = false;

    if (!f)
        return false;

    if (!yaml_parser_initialize(&parser))
    {
        fputs("初始化解析器失败\n", stderr);
        return 1;
    }
    yaml_parser_set_input_file(&parser, f);

    do
    {
        if (!yaml_parser_parse(&parser, &event))
        {
            fprintf(stderr, "解析错误: %d\n", parser.error);
            return 1;
        }

        switch (event.type)
        {
        case YAML_SCALAR_EVENT:
            if (strncmp(key, event.data.scalar.value, strlen(key))==0)
                found = true;
            else if (found && str_casecmp(event.data.scalar.value, "true") == 0)
                return true;
            else if (found && str_casecmp(event.data.scalar.value, "false") == 0)
                return false;
            break;
        default:
            break;
        }

        yaml_event_delete(&event);
    } while (event.type != YAML_STREAM_END_EVENT);

    yaml_parser_delete(&parser);
    fclose(f);
    return false;
}