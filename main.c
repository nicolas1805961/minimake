#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

int main(int argc, char *argv[])
{
    struct parser *parser = malloc(sizeof(struct parser));
    int t = 0;
    int y = 0;
    int return_token = 0;
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
    if (t > 1)
    {
        printf("This is the help for minimake\n");
        return 0;
    }
    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'f':
                if (argv[i][2] == 'h')
                {
                    printf("This is the help for minimake\n");
                    return 0;
                }
                return parse(argv[i + 1], parser);
                break;
            case 'h':
                printf("This is the help for minimake\n");
                return 0;
                break;
            default:
                return_token = parse("makefile", parser);
                if (return_token != 0)
                    return parse("Makefile", parser);
                break;
            }
        }
    }
}
