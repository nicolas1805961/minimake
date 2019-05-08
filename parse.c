#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

char *get_tokens(FILE *fp, char *tokens[], size_t size)
{
    int i = 1;
    char *line = NULL;
    while (line = my_getline(fp))
    {
        char *ptr = strchr(line, ':');
        if (ptr != NULL)
        {   
            char *temp;
            char *target = strtok(line,':');
            strncpy(tokens[0], target, 64);
            while (i < size && temp = strtok(NULL, ' '))
            {
                strncpy(tokens[i], temp, 64);
                i++;
            }
            return tokens;
        }
    } 
}

char *my_getline(FILE *fp)
{
    int x = 0;
    char line[4096];
    fgets(line, 4096, fp);
    char *ptr = strchr(line, '\n');
    if (ptr != NULL)
        *ptr = '\0';
    else
    {
        while (x != 0 && x != EOF)
            x = getchar();
    }
    return line;
}

int parse(char *file, char *rules[])
{
    char tokens[64][64];
    FILE *fp = fopen(file, 'r');
    if (fp == NULL)
        return 1;
    get_tokens(fp, tokens, 64);
    
}