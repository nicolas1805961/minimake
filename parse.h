#include <stdio.h>

int parse(char *file, char *rules[]);
char *my_getline(FILE *fp);
char *get_tokens(FILE *fp, char *tokens[], size_t size);