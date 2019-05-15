#pragma once
#include <stdio.h>
#include <sys/types.h>

struct parser
{
    struct variable_list *list;
    char *commands;
    char *rule_executed;
    char *tokens[64];
    char *rules[64];
    int nb_rules;
    int nb_tokens;
    char *line1;
    char *line2;
    char *file;
    FILE *fp1;
    FILE *fp2;
};

int parse(char *file, struct parser *parser);
int my_getlines(struct parser *parser);
int my_getline(struct parser *parser);
int get_tokens(struct parser *parser, char *rule);
int parse_file(struct parser *parser, char *rule);
int check_tab(char *line);
int execute(char *command);
void free_function(struct parser *parser);
int init_main(int argc, char *argv[], struct parser *parser);
int display_help(struct parser *parser);
int search(struct parser *parser, char *rule);
int check_rule(char *line);
int check_not_executed(char *rule, struct parser *parser);
int setup_execution(struct parser *parser, char *rule);
struct parser *copy_struct(struct parser *src);
struct parser *copy_struct_no_alloc(struct parser *dest, struct parser *src);
void handle_comments(char *line);
int is_main_rule(char *rule, struct parser *parser);
int check_modification_date(struct parser *parser, char *token);
int check_modification_date_loop(struct parser *parser);
int check_proper(struct parser *parser, int flag, char *rule);
int check_same(struct parser *parser, char *rule);
void delete_c(char *command);
int get_token_tool(struct parser *parser, char *target, char *line);
int parse_tool(struct parser *parser);
int parent_process(pid_t pid, int status);
void copy_list(struct parser *dest, struct parser *src);
void copy_list_no_alloc(struct parser *dest, struct parser *src);