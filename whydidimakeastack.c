struct IntStackNode
{
    int value;
    int_node_t *next;
};


int_node_t *file_to_stack(FILE *file)
{
    int_node_t *int_stack_head = NULL;
    int c = 0;
    for (;;)
    {
        c = getc(file);
        if (c == EOF)
        {
            return int_stack_head;
        }
        else
        {
            int_node_push(c, int_stack_head);
        }
    }
}

int int_node_pop(int_node_t *head)
{
    if (head == NULL)
    {
        return EOF;
    }
    else
    {
        int_node_t *tmp;
        int tmp_int = head->value;
        tmp = head;
        head = head->next;
        free(tmp);
        return tmp_int;
    }
}

void int_node_push(int value, int_node_t *head)
{
    int_node_t *new_node = malloc(sizeof(int_node_t));
    new_node->value = value;
    new_node->next = head;
    head = new_node;
}
