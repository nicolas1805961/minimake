#pragma once
#include <stdio.h>

struct parser
{
    char *commands[64];
    char *tokens[64];
    char *rules[64];
    int nb_rules;
    char *line;
};

int parse(char *file, struct parser *parser);
int my_getline(FILE *fp, struct parser *parser);
int get_tokens(struct parser *parser);
void parse_file(FILE *fp, struct parser *parser);
int check_tab(struct parser *parser);