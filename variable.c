#include "variable.h"
#include <string.h>
#include <stdlib.h>

void delete_all(struct variable_list *vl)
{
    struct variable_node *temp = vl->head;
    struct variable_node *temp2 = NULL;
    while (temp)
    {
        temp2 = temp;
        temp = temp->next;
        free(temp2->left);
        free(temp2->right);
        free(temp2);
    }
    vl->head = NULL;
}

void add(char *left, char *right, struct variable_list *vl)
{
    struct variable_node *new = malloc(sizeof(struct variable_node));
    new->left = malloc(64);
    strncpy(new->left, left, 64);
    new->right = malloc(64);
    if (!right)
        strncpy(new->right, "", 64);
    else
        strncpy(new->right, right, 64);
    new->next = NULL;
    if (vl->size == 0)
        vl->head = new;
    else
    {
        struct variable_node *temp = vl->head;
        while (temp->next)
            temp = temp->next;
        temp->next = new;
    }
    vl->size++;
}
char *get(struct parser *parser, char *left, struct variable_list *vl, int *q)
{
    if (!strncmp(left, "@", 2))
        return get_recursive_variable(parser, parser->tokens[0], vl);
    if (!strncmp(left, "<", 2))
        return get_recursive_variable(parser, parser->tokens[1], vl);
    if (!strncmp(left, "^", 2))
    {
        *q = 1;
        char *right = malloc(1024);
        strncpy(right, parser->tokens[1], 64);
        for (int i = 2; i < parser->nb_tokens; i++)
        {
            strncat(right, " ", 2);
            strncat(right, parser->tokens[i], 64);
        }
        while (check_todo(right))
            replace(parser, right, parser->list);
        return right;
    }
    struct variable_node *temp = vl->head;
    while (temp)
    {
        if (!strncmp(temp->left, left, 64))
            return temp->right;
        temp = temp->next;
    }
    return NULL;
}
void free_list(struct variable_list *vl)
{
    struct variable_node *temp = vl->head;
    struct variable_node *temp2 = NULL;
    while (temp)
    {
        temp2 = temp;
        temp = temp->next;
        free(temp2->left);
        free(temp2->right);
        free(temp2);
    }
    free(vl);
}
struct variable_list *init()
{
    struct variable_list *list = malloc(sizeof(struct variable_list));
    list->head = NULL;
    list->size = 0;
    return list;
}
void parse_variables(struct parser *parser)
{
    struct variable_list *list = init();
    parser->list = list;
    FILE *fp = fopen(parser->file, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file.\n");
        free_function(parser);
        exit(1);
    }
    parser->fp1 = fp;
    FILE *fd = fopen(parser->file, "r");
    if (fd == NULL)
    {
        fprintf(stderr, "Could not open file.\n");
        free_function(parser);
        exit(1);
    }
    parser->fp2 = fd;
    get_variables(parser, parser->list);
}

char *get_recursive_variable(struct parser *parser, char *right, struct variable_list *list)
{
    if (right && check_todo(right))
    {
        int q = 0;
        int p = 0;
        int r = 0;
        int *ptr = NULL;
        if (right[1] != '(')
            p = 1;
        else
        {
            p = 2;
            while (right[r] != '}' && right[r] != ')')
                r++;
        }
        ptr = &p;
        shift(right, ptr, r);
        char *right_init = get(parser, right, list, &q);
        return get_recursive_variable(parser, right_init, list);
    }
    return right;
}

void replace(struct parser *parser, char *line, struct variable_list *list)
{
    int i = 0;
    int j = 0;
    int q = 0;
    char *variable = get_left(line, &i, &j);
    char *right_init = get(parser, variable, list, &q);
    char *right = get_recursive_variable(parser, right_init, list);
    if (right)
    {
        shift(line, &i, j);
        append(line, i, right);
    }
    else
    {
        char *env = getenv(variable);
        if (env)
        {
            shift(line, &i, j);
            append(line, i, env);
        }
        else
        {
            fprintf(stderr, "No corresponding value for variable '%s' found\n", variable);
            free(variable);
            if (q == 1)
                free(right_init);
            exit(1);
        }
    }
    free(variable);
    if (q == 1)
        free(right_init);
}

int check_todo(char *line)
{
    int y = 0;
    while (line[y])
    {
        if (line[y] == '=')
            return 0;
        y++;
    }
    int i = 0;
    while (line[i] != '\0' && line[i] != '$')
        i++;
    if (line[i] == '$' && (line[i + 1] == '(' || line[i + 1] == '{'))
    {
        int j = i;
        while (line[j] != ')' && line[j] != '}')
            j++;
        if (line[j] == ')' || line[j] == '}')
            return 1;
        else
        {
            fprintf(stderr, "Invalid syntax for variable.\n");
            exit(1);
        }
    }
    else if (line[i] == '$' && (line[i + 2] == ' ' || line[i + 2] == '\0'))
        return 1;
    return 0;
}

char *get_left(char *line, int *i, int *j)
{
    while (line[(*i)] != '$')
        (*i)++;
    if (line[*i + 2] == ' ' || line[*i + 2] == '\0')
        return one_char_get_left(i, line);
    if (line[(*i) + 1] == '(' || line[(*i) + 1] == '{')
        return several_chars_get_left(i, j, line);
    return NULL;
}

char *one_char_get_left(int *i, char *line)
{
    (*i)++;
    char *variable = malloc(2 * sizeof(char));
    variable[0] = line[*i];
    variable[1] = '\0';
    return variable;
}

char *several_chars_get_left(int *i, int *j, char *line)
{
    (*i) += 2;
    (*j) = (*i);
    while (line[(*j)] != ')' && line[(*j)] != '}')
        (*j)++;
    int t = (*j) - (*i);
    char *variable = malloc(t + 1);
    int u = (*i);
    int y = 0;
    while (u != (*j))
    {
        variable[y] = line[u];
        u++;
        y++;
    }
    variable[y] = '\0';
    return variable;
}

void append(char *line, int i, char *right)
{
    int j = i;
    while (line[i] != ' ' && line[i] != '\0')
        i++;
    if (line[i] == '\0')
    {
        while (line[i] != ' ')
            i--;
        i++;
        strncpy(line + i, right, 64);
    }
    else
        append_tool(line, i, right, j);
}

void append_tool(char *line, int i, char *right, int j)
{
    i++;
    char *save = malloc(1024);
    char *s = strtok(line + i, "\0");
    strncpy(save, s, 1024);
    size_t p = strlen(right);
    strncpy(line + j, right, p);
    if (p > 0)
    {
        line[j + p] = ' ';
        line[j + p + 1] = '\0';
    }
    else
        line[j] = '\0';
    strncat(line, save, 1024);
    free(save);
}

void shift(char *line, int *i, int j)
{
    if (line[*i - 1] == '$')
    {
        one_char_shift(line, i, j);
        return;
    }
    several_chars_shift(line, i, j);
    return;
}

void one_char_shift(char *line, int *i, int j)
{
    j = *i;
    while (line[*i])
    {
        line[*i - 1] = line[*i];
        (*i)++;
    }
    line[(*i) - 1] = '\0';
    j--;
    while (*i != j)
        (*i)--;
    return;
}

void several_chars_shift(char *line, int *i, int j)
{
    while (line[(*i)])
    {
        line[(*i) - 2] = line[(*i)];
        (*i)++;
    }
    line[(*i) - 2] = '\0';
    j -= 2;
    while (*i != j)
        (*i)--;
    while (*i != 0 && line[*i] != ' ')
        (*i)--;
    if (*i != 0)
        (*i)++;
    while (line[j + 1])
    {
        line[j] = line[j + 1];
        j++;
    }
    line[j] = '\0';
}

int check_variable(char *line)
{
    char *ptr = NULL;
    ptr = strchr(line, '=');
    if (ptr != NULL)
        return 1;
    else
        return 0;
}
void get_variables(struct parser *parser, struct variable_list *list)
{
    while (my_getlines(parser))
    {
        char *line = malloc(1024);
        strncpy(line, parser->line1, 1024);
        if (check_variable(parser->line1))
        {
            char *left = strtok(parser->line1, "=");
            truncate_right(left);
            char *right = strtok(NULL, "\0");
            truncate_left(right);
            add(left, right, list);
        }
        strncpy(parser->line1, line, 1024);
        free(line);
    }
}

void truncate_right(char *line)
{
    int i = 0;
    while (line[i] != ' ' && line[i] != '\0')
        i++;
    if (line[i] == ' ')
        line[i] = '\0';
}

void truncate_left(char *line)
{
    if (!line)
        return;
    int i = 0;
    int j = 0;
    while (line[i] == ' ')
        i++;
    while (line[i])
    {
        line[j] = line[i];
        j++;
        i++;
    }
    line[j] = '\0';
}