#pragma once
#include <stdio.h>

struct parser
{
    char *commands;
    char *tokens[64];
    char *rules[64];
    char *rules_executed[64];
    int nb_rules;
    int nb_rules_executed;
    int nb_tokens;
    char *line;
    char *file;
    FILE *fp;
};

int parse(char *file, struct parser *parser);
int my_getline(struct parser *parser);
int get_tokens(struct parser *parser, char *rule);
int parse_file(struct parser *parser, char *rule);
int check_tab(struct parser *parser);
int execute(char *command);
void free_function(struct parser *parser);
int init_main(int argc, char *argv[], struct parser *parser);
int display_help(struct parser *parser);
int search(struct parser *parser, char *rule);
int check_rule(struct parser *parser);
int check_not_executed(struct parser *parser, char *rule);
int setup_execution(struct parser *parser, char *rule);
