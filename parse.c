#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
struct parser *copy_struct_no_alloc(struct parser *dest, struct parser *src)
{
    for (int i = 0; i < src->nb_rules; i++)
        strncpy(dest->rules[i], src->rules[i], 64);
    strncpy(dest->file, src->file, 64);
    strncpy(dest->line1, src->line1, 1024);
    strncpy(dest->line2, src->line2, 1024);
    strncpy(dest->commands, src->commands, 1024);
    strncpy(dest->rule_executed, src->rule_executed, 64);
    for (int i = 0; i < src->nb_tokens; i++)
        strncpy(dest->tokens[i], src->tokens[i], 64);
    dest->nb_rules = src->nb_rules;
    dest->nb_tokens = src->nb_tokens;
    dest->fp1 = src->fp1;
    dest->fp2 = src->fp2;
    return dest;
}
struct parser *copy_struct(struct parser *src)
{
    struct parser *dest = malloc(sizeof(struct parser));
    for(int i = 0; i < 64; i++)
        dest->rules[i] = malloc(64);
    dest->file = malloc(64);
    dest->line1 = malloc(1024);
    dest->line2 = malloc(1024);
    dest->commands = malloc(1024);
    dest->rule_executed = malloc(64);
    for (int i = 0; i < 64; i++)
        dest->tokens[i] = malloc(64);
    for (int i = 0; i < src->nb_rules; i++)
        strncpy(dest->rules[i], src->rules[i], 64);
    strncpy(dest->rule_executed, src->rule_executed, 64);
    strncpy(dest->file, src->file, 64);
    strncpy(dest->line1, src->line1, 1024);
    strncpy(dest->line2, src->line2, 1024);
    strncpy(dest->commands, src->commands, 1024);
    for (int i = 0; i < src->nb_tokens; i++)
        strncpy(dest->tokens[i], src->tokens[i], 64);
    dest->nb_rules = src->nb_rules;
    dest->nb_tokens = src->nb_tokens;
    dest->fp1 = src->fp1;
    dest->fp2 = src->fp2;
    return dest;
}
int check_rule(char *line)
{
    char *ptr = NULL;
    ptr = strchr(line, ':');
    if (ptr != NULL)
        return 1;
    else
        return 0;
}
int get_tokens(struct parser *parser, char *rule)
{
    char *line = malloc(1024);
    strncpy(line, parser->line1, 1024);
    if (check_rule(parser->line1))
    {   
        char *temp = NULL;
        char *target = strtok(parser->line1,":");
        if (rule && (strncmp(target, rule, 64) != 0))
        {
            strncpy(parser->line1, line, 1024);
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
            strncpy(parser->line1, line, 1024);
            free(line);
            return 1;
        }
    }
    strncpy(parser->line1, line, 1024);
    free(line);
    return 0;
}
int check_tab(char *line)
{
    for (int i = 0; i < 4; i++)
    {
        if (line[i] != ' ')
            return 0;
    }
    if (line[4] != ' ')
        return 1;
    return 0;
}
void handle_comments(char *line)
{
    char *ptr1 = NULL;
    if (!check_tab(line) && (ptr1 = strchr(line, '#')))
        *ptr1 = '\0';
    char *ptr2 = strchr(line, '\n');
    if (ptr2 != NULL)
        *ptr2 = '\0';
}
int my_getline(struct parser *parser, int n)
{
    char *line2 = malloc(1024);
    strncpy(line2, parser->line2, 1024);
    fgets(parser->line2, 1024, parser->fp2);
    if (n == 2)
        fgets(parser->line1, 1024, parser->fp1);
    if (!strncmp(parser->line2, line2, 1024))
    {
        free(line2);
        return 0;
    }
    handle_comments(parser->line2);
    if (n == 2)
        handle_comments(parser->line1);
    free(line2);
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
    parser->fp1 = fp;
    FILE *fd = fopen(parser->file, "r");
    if (fd == NULL)
    {
        free_function(parser);
        return 0;
    }
    parser->fp2 = fd;
    while (my_getline(parser, 2))
    {
        if (get_tokens(parser, rule))
            return 1;
    }
    return 0;
}

int check_not_executed(char *rule, struct parser *parser)
{
    if (is_main_rule(rule, parser) && !strncmp(rule, "all", 4))
    {
        printf("minimake: '%s' is up to date.\n", rule);
        return 1;
    }
    return 0;
}

int check_modification_date_loop(struct parser *parser)
{
    struct stat st;
    struct stat sp;
    if (!strncmp(parser->tokens[0], "all", 4))
        return 0;
    if (stat(parser->tokens[0], &st) == -1)
        return 1;
    time_t target_md = st.st_mtime;
    for (int i = 0; i < parser->nb_tokens; i++)
    {
        stat(parser->tokens[i], &sp);
        time_t dependency_md = sp.st_mtime;
        if (difftime(target_md, dependency_md) < 0)
            return 1;
    }
    return 0;
}

int parse_file(struct parser *parser, char *rule)
{
    int i = 1;
    if (search(parser, rule))
    {
        while (i < parser->nb_tokens)
        {
            if (check_modification_date(parser, parser->tokens[i]))
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
        if (check_modification_date_loop(parser))
            setup_execution(parser, parser->tokens[0]);
        return 0;
    }
    else
    {
        printf("minimake: no rule to make target '%s'\n", rule);
        exit(1); 
    }
}
int is_main_rule(char *rule, struct parser *parser)
{
    for (int i = 0; i < parser->nb_rules; i++)
    {
        if (!strncmp(rule, parser->rules[i], 64))
            return 1;
    }
    return 0;
}
int check_modification_date(struct parser *parser, char *token)
{
    struct stat st;
    struct stat sp;
    if (!strncmp(parser->tokens[0], "all", 4))
        return 0;
    if (stat(token, &sp) == -1)
        return 0;
    else
    {
        if (stat(parser->tokens[0], &st) == -1)
            return 1;
        time_t target_md = st.st_mtime;
        time_t dependency_md = sp.st_mtime;
        if (difftime(target_md, dependency_md) < 0)
            return 1;
    }
     return 0;
}
int check_proper(struct parser *parser, int flag, char *rule)
{
    if (parser->line2[0] == '\0')
    {
        printf("minimake: Nothing to be done for '%s'.\n", rule);
        exit(1);
    }
    if (check_rule(parser->line2))
    {
        if (flag == 0)
        {
            printf("minimake: Nothing to be done for '%s'.\n", rule);
            exit(1);
        }
        return 0;
    }
    return 1;
}
int setup_execution(struct parser *parser, char *rule)
{
    int flag = 0;
    /*if (check_not_executed(rule, parser))
        return 1;*/
    while (my_getline(parser, 1))
    {
        if (!check_proper(parser, flag, rule))
            break;
        for (size_t t = 0; t < 4; t++)
        {
            for (size_t i = 0; i < strlen(parser->line2); i++)
                parser->line2[i] = parser->line2[i + 1];
        }
        strncpy(parser->commands, parser->line2, 1024);
        if (parser->commands[0] != '@')
            printf("%s\n", parser->commands);
        if (!execute(parser->commands))
            return 2;
        flag = 1;
    }
    return 0;
}

int parse(char *file, struct parser *parser)
{
    parser->file = malloc(64);
    parser->rule_executed = malloc(64);
    strncpy(parser->file, file, 64);
    int return_value = 0;
    parser->line1 = malloc(1024);
    parser->line2 = malloc(1024);
    parser->commands = malloc(1024);
    for (int i = 0; i < 64; i++)
        parser->tokens[i] = malloc(64);
    if (parser->nb_rules > 0)
    {
        for (int i = 0; i < parser->nb_rules; i++)
        {
            for (int j = 0; j < i; i++)
            {
                if (!strncmp(parser->rules[i], parser->rules[j], 64))
                {
                    printf("minimake: '%s' is up to date.\n", parser->rules[i]);
                    free_function(parser);
                    return 0;
                }
            }
            strncpy(parser->rule_executed, parser->rules[i], 64);
            return_value = parse_file(parser, parser->rules[i]);
        }
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
    free(parser->line1);
    free(parser->line2);
    free(parser->file);
    free(parser->rule_executed);
    free(parser);
}
