#include "variable.h"
#include <string.h>
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
struct variable_node *get(char *left)
{
    struct variable_node *temp = vl->head;
    while (temp)
    {
        if (!strncmp(temp->left, temp, 64))
            return temp;
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
    list->size = 0;
}
void parse_variables(struct parser *parser)
{

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
int get_variables(struct parser *parser, struct variable_list *list)
{
    while (my_getline(parser, 2))
    {
        char *line = malloc(1024);
        strncpy(line, parser->line1, 1024);
        if (check_variable(parser->line1))
        {   
            char *temp = NULL;
            char *left = strtok(parser->line1, "=");
            char *right = strtok(parser->line1, "\0");
            add(left, right);
            strncpy(parser->line1, line, 1024);
            free(line);
            return 1;
        }
        strncpy(parser->line1, line, 1024);
        free(line);
        return 0;
    }
    
}