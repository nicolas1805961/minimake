#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

int main(int argc, char *argv[])
{
    int return_token = 0;
    struct parser *parser = malloc(sizeof(struct parser));
    if (init_main(argc, argv, parser) > 1)
        return display_help(parser);
    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'f':
                return parse(argv[i + 1], parser);
                break;
            case 'h':
                return display_help(parser);
                break;
            default:
                break;
            }
        }
    }
    return_token = parse("makefile", parser);
    if (return_token != 0)
    {
        parser = malloc(sizeof(struct parser));
        for(int i = 0; i < 64; i++)
            parser->rules[i] = malloc(64);
        return parse("Makefile", parser);
    }
    return return_token;
}

int init_main(int argc, char *argv[], struct parser *parser)
{
    int t = 0;
    int y = 0;
    for(int i = 0; i < 64; i++)
        parser->rules[i] = malloc(64);
    for (int j = 1; j < argc; j++)
    {
        if (!strncmp(argv[j], "-f", 2) || !strncmp(argv[j], "-h", 2))
            t++;
        if (argv[j -1] && argv[j - 1][0] != '-' && argv[j - 1][0] != '.' && argv[j - 1][0] != '/')
        {
            strncpy(parser->rules[y], argv[j], 64);
            y++;
        }
    }
    parser->nb_rules = y;
    return t;
}

int display_help(struct parser *parser)
{
    printf("This is the help for minimake\n");
    free(parser);
    for (int i = 0; i < 64; i++)
        free(parser->rules[i]);
    return 0;
}
