#pragma once
#include "parse.h"

struct variable_node
{
    struct variable_node *next;
    char *left;
    char *right;
};
struct variable_list
{
    struct variable_node *head;
    int size;
};
void add(char *left, char *right, struct variable_list *vl);
char *get(struct parser *parser, char *left, struct variable_list *vl, int *q);
void free_list(struct variable_list *vl);
struct variable_list *init();
void parse_variables(struct parser *parser);
int check_variable(char *line);
void get_variables(struct parser *parser, struct variable_list *list);
void replace(struct parser *parser, char *line, struct variable_list *list);
void shift(char *line, int *i, int j);
void append(char *line, int i, char *right);
char *get_left(char *line, int *i, int *j);
int check_todo(char *line);
void truncate_right(char *line);
void truncate_left(char *line);
char *get_recursive_variable(struct parser *parser, char *right, struct variable_list *list);
void append_tool(char *line, int i, char *right, int j);
void delete_all(struct variable_list *vl);
char *one_char_get_left(int *i, char *line);
char *several_chars_get_left(int *i, int *j, char *line);
void one_char_shift(char *line, int *i, int j);
void several_chars_shift(char *line, int *i, int j);