#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
struct parser *copy_struct_no_alloc(struct parser *dest, struct parser *src)
{
    for (int i = 0; i < src->nb_rules; i++)
        strncpy(dest->rules[i], src->rules[i], 64);
    strncpy(dest->file, src->file, 64);
    strncpy(dest->line, src->line, 1024);
    strncpy(dest->commands, src->commands, 1024);
    for (int i = 0; i < src->nb_tokens; i++)
        strncpy(dest->tokens[i], src->tokens[i], 64);
    dest->nb_rules = src->nb_rules;
    dest->nb_tokens = src->nb_tokens;
    dest->fp = src->fp;
    return dest;
}
struct parser *copy_struct(struct parser *src)
{
    struct parser *dest = malloc(sizeof(struct parser));
    for(int i = 0; i < 64; i++)
        dest->rules[i] = malloc(64);
    dest->file = malloc(64);
    dest->line = malloc(1024);
    dest->commands = malloc(1024);
    for (int i = 0; i < 64; i++)
        dest->tokens[i] = malloc(64);
    for (int i = 0; i < src->nb_rules; i++)
        strncpy(dest->rules[i], src->rules[i], 64);
    strncpy(dest->file, src->file, 64);
    strncpy(dest->line, src->line, 1024);
    strncpy(dest->commands, src->commands, 1024);
    for (int i = 0; i < src->nb_tokens; i++)
        strncpy(dest->tokens[i], src->tokens[i], 64);
    dest->nb_rules = src->nb_rules;
    dest->nb_tokens = src->nb_tokens;
    dest->fp = src->fp;
    return dest;
}
int check_rule(struct parser *parser)
{
    char *ptr = NULL;
    ptr = strchr(parser->line, ':');
    if (ptr != NULL)
        return 1;
    else
        return 0;
}
int get_tokens(struct parser *parser, char *rule)
{
    char *line = malloc(1024);
    strncpy(line, parser->line, 1024);
    if (check_rule(parser))
    {   
        char *temp = NULL;
        char *target = strtok(parser->line,":");
        if (rule && (strncmp(target, rule, 64) != 0))
        {
            strncpy(parser->line, line, 1024);
            free(line);
            return 0;
        }
        else
        {
            parser->nb_tokens = 0;
            strncpy(parser->tokens[0], target, 64);
            parser->nb_tokens++;
            while ((parser->nb_tokens < 64) && (temp = strtok(NULL, " ")))
            {
                strncpy(parser->tokens[parser->nb_tokens], temp, 64);
                parser->nb_tokens++;
            }
            strncpy(parser->line, line, 1024);
            free(line);
            return 1;
        }
    }
    strncpy(parser->line, line, 1024);
    free(line);
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
int my_getline(struct parser *parser)
{
    char *line = malloc(1024);
    strncpy(line, parser->line, 1024);
    fgets(parser->line, 1024, parser->fp);
    if (!strncmp(parser->line, line, 1024))
    {
        free(line);
        return 0;
    }
    char *ptr1 = NULL;
    if (!check_tab(parser) && (ptr1 = strchr(parser->line, '#')))
        *ptr1 = '\0';
    char *ptr2 = strchr(parser->line, '\n');
    if (ptr2 != NULL)
        *ptr2 = '\0';
    free(line);
    return 1;
}

int execute(char *command)
{
    int return_value = 0;
    int status;
    pid_t pid;
    pid_t w;
    /*int c = 0;
    while (c != '\n' && c != EOF)
        c = getchar();*/
    pid = fork();
    if (pid == 0)
    {
        return_value = execlp("/bin/sh","/bin/sh", "-c", command, (char *)NULL);
        if (return_value == -1)
        {
            fprintf(stderr, "Error in execution of command: %s\n", command);
            return 0;
        }
    }
    else if (pid > 0)
    {
        w = waitpid(pid, &status, 0);
        if (w == -1) 
        {
            perror("waitpid");
            return 0;
        }
        if (WIFEXITED(status)) 
            printf("exited, status=%d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("killed by signal %d\n", WTERMSIG(status));
        else if (WIFSTOPPED(status))
            printf("stopped by signal %d\n", WSTOPSIG(status));
    }
    else
    {
        perror("fork");
        return 0;
    }
    return 1;
}

int search(struct parser *parser, char *rule)
{
    FILE *fp = fopen(parser->file, "r");
    if (fp == NULL)
    {
        free_function(parser);
        return 0;
    }
    parser->fp = fp;
    while (my_getline(parser))
    {
        if (get_tokens(parser, rule))
            return 1;
    }
    return 0;
}

int check_not_executed(char *rule)
{
    struct stat st;
    if (!stat(rule, &st) || !strncmp(rule, "all", 4))
    {
        printf("minimake: '%s' is up to date\n", rule);
            exit(1);
    }
    return 0;
}

int parse_file(struct parser *parser, char *rule)
{
    int i = 1;
    if (search(parser, rule))
    {
        struct stat st;
        while (i < parser->nb_tokens)
        {
            if (!stat(parser->tokens[i], &st))
                setup_execution(parser, parser->tokens[0]);
            else
            {
                struct parser *saved_parser = copy_struct(parser);
                parse_file(parser, parser->tokens[i]);
                parser = copy_struct_no_alloc(parser, saved_parser);
                free_function(saved_parser);
            }
            i++;
        }
        if (strncmp(parser->tokens[0], "all", 64) != 0 && stat(parser->tokens[0], &st) == -1)
            setup_execution(parser, parser->tokens[0]);
        return 0;
    }
    else
    {
        printf("minimake: no rule to make target '%s'\n", rule);
        exit(1);
    }
}

int setup_execution(struct parser *parser, char *rule)
{
    if (check_not_executed(rule))
        return 1;
    while (my_getline(parser))
    {
        if(check_rule(parser))
            return 1;
        if (parser->line[0] == '\0')
            continue;
        for (size_t t = 0; t < 4; t++)
        {
            for (size_t i = 0; i < strlen(parser->line); i++)
                parser->line[i] = parser->line[i + 1];
        }
        strncpy(parser->commands, parser->line, 1024);
        if (parser->commands[0] != '@')
            printf("%s\n", parser->commands);
        if (!execute(parser->commands))
            return 2;
    }
    return 0;
}

int parse(char *file, struct parser *parser)
{
    parser->file = malloc(64);
    strncpy(parser->file, file, 64);
    int return_value = 0;
    parser->line = malloc(1024);
    parser->commands = malloc(1024);
    for (int i = 0; i < 64; i++)
        parser->tokens[i] = malloc(64);
    if (parser->nb_rules > 0)
    {
        for (int i = 0; i < parser->nb_rules; i++)
        return_value = parse_file(parser, parser->rules[i]);
    }
    else
        return_value = parse_file(parser, NULL);
    free_function(parser);
    return return_value;
}

void free_function(struct parser *parser)
{
    for (int i = 0; i < 64; i++)
    {
        free(parser->tokens[i]);
        free(parser->rules[i]);
    }
    free(parser->commands);
    free(parser->line);
    free(parser->file);
    free(parser);
}
