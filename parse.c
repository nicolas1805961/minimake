#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

int get_tokens(struct parser *parser)
{
    size_t i = 1;
    char *ptr = NULL;
    ptr = strchr(parser->line, ':');
    if (ptr != NULL)
    {   
        char *temp = NULL;
        char *target = strtok(parser->line,":");
        strncpy(parser->tokens[0], target, 64);
        while ((i < 64) && (temp = strtok(NULL, " ")))
        {
            strncpy(parser->tokens[i], temp, 64);
            i++;
        }
        return 1;
    }
    return 0;
}
int check_tab(struct parser *parser)
{
    for (int i = 0; i < 4; i++)
    {
        if (parser->line[i] != ' ')
            return 0;
    }
    if (parser->line[4] != ' ')
        return 1;
    return 0;
}
int my_getline(FILE *fp, struct parser *parser)
{
    parser->line = fgets(parser->line, 1024, fp);
    if (parser->line == NULL)
        return 0;
    char *ptr1 = NULL;
    if (!check_tab(parser) && (ptr1 = strchr(parser->line, '#')))
        *ptr1 = '\0';
    char *ptr2 = strchr(parser->line, '\n');
    if (ptr2 != NULL)
        *ptr2 = '\0';
    return 1;
}

void parse_file(FILE *fp, struct parser *parser)
{
    int j = 0;
    int u = 0;
    while(my_getline(fp, parser))
    {
        /*if (u > parser->nb_rules)
            return;*/
        if (parser->line[0] == '\0')
            continue;
        if (!get_tokens(parser))
        {
            if (parser->nb_rules != 0 && parser->tokens[0] != parser->rules[u])
                continue;
            for (size_t t = 0; t < 4; t++)
            {
                for (size_t i = 0; i < strlen(parser->line); i++)
                    parser->line[i] = parser->line[i + 1];
            }
            strncpy(parser->commands[j], parser->line, 1024);
            printf("%s\n", parser->commands[j]);
            j++;
        }
        else
        {
            if (j > 0)
                u++;
            j = 0;
        }
    }
}
int parse(char *file, struct parser *parser)
{
    parser->line = malloc(1024);
    for (int i = 0; i < 64; i++)
    {
        parser->tokens[i] = malloc(64);
        parser->commands[i] = malloc(1024);
    }
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
        return 1;
    parse_file(fp, parser);
    for (int i = 0; i < 64; i++)
        free(parser->tokens[i]);
    for (int i = 0; i < 64; i++)
        free(parser->commands[i]);
    for (int i = 0; i < 64; i++)
        free(parser->rules[i]);
    free(parser->line);
    free(parser);
    return 0;
}