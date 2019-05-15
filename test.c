#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "variable.h"

void appendd(char *line, int i, char *right)
{
    int j = i;
    while (line[i] != ' ' && line[i] != '\0')
        i++;
    if (line[i] == '\0')
        return;
    else
    {
        i++;
        char *s = strtok(line + i, "\0");
        size_t p = strlen(right);
        strncpy(line + j, right, p);
        line[j + p] = ' ';
        line[j + p + 1] = '\0';
        strncat(line, s, 64);
    }
}

char *get_leftt(char *line, int *i, int *j)
{
    int y = 0;
    while (line[(*i)] != '$')
        (*i)++;
    if (line[(*i) + 1] == '(' || line[(*i) + 1] == '{')
        (*i) += 2;
    (*j) = (*i);
    while (line[(*j)] != ')' && line[(*j)] != '}')
        (*j)++;
    int t = (*j) - (*i);
    char *variable = malloc(t);
    int u = (*i);
    while (u != (*j))
    {
        variable[y] = line[u];
        u++;
        y++;
    }
    variable[y] = '\0';
    return variable;
}

void shiftt(char *line, int *i, int j)
{
    while (line[(*i)])
    {
        line[(*i) - 2] = line[(*i)];
        (*i)++;
    }
    line[(*i) - 2] = '\0';
    j -= 2;
    while (line[j + 1])
    {
        line[j] = line[j + 1];
        j++;
    }
    line[j] = '\0';
}

void replacee(char *line, struct variable_list *list)
{
    int i = 0;
    int j = 0;
    char *variable = get_leftt(line, &i, &j);
    char *right = get(variable, list);
    if (right)
    {
        shiftt(line, &i, j);
        i -=2;
        appendd(line, i, right);
    }
}

char *get(char *left, struct variable_list *vl)
{
    struct variable_node *temp = vl->head;
    while (temp)
    {
        if (!strncmp(temp->left, left, 64))
            return temp->right;
        temp = temp->next;
    }
    return NULL;
}

void add(char *left, char *right, struct variable_list *vl)
{
    struct variable_node *new = malloc(sizeof(struct variable_node));
    new->left = malloc(64);
    new->right = malloc(64);
    strncpy(new->left, left, 64);
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

struct variable_list *init()
{
    struct variable_list *list = malloc(sizeof(struct variable_list));
    list->size = 0;
    return list;
}

int main(int argc, char const *argv[])
{
    struct variable_list *list = init();
    add("poo", "poop", list);
    char *string = malloc(64);
    strncpy(string, "allo me too $(poo) titi", 64);
    replacee(string, list);
    printf("%s\n", string);
    free(string);
    return 0;
}
//allo me too poop