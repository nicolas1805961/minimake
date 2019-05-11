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
void add(char *left, char *right);
struct variable_node *get(char *left);
void free_list();
struct variable_list *init();
void parse_variables(struct parser *parser);
int check_variable(char *line);
int get_variables(struct parser *parser, struct variable_list *list);