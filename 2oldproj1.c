#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "proj1.h"
#include <time.h>

macro_node_t *head = NULL;
macro_node_t *current = NULL;
string_t *combined_input_string = string_malloc(16);

typedef enum
{
    state_plaintext = 0,
    state_escape = 1,
    state_read_macro_name = 2,
} State;

struct String
{
    char *data;
    long long size;
    long long capacity;
};

struct UserMacro
{
    string_t *name;
    string_t *value;
    macro_node_t *next;
};

// print error message and exit program
void error_message(int line)
{
    fprintf(stderr, "Processing Error Detected on line [%d]\n", line);
    exit(1);
}

// free macro node
void macro_node_free(macro_node_t *node)
{
    string_free(node->name);
    string_free(node->value);
    free(node);
    return;
}
// returns user macro node with given macro name and NULL can't be found
macro_node_t *macro_node_find(string_t *name)
{
    current = head;

    if (head == NULL)
    {
        return NULL;
    }

    while (strcmp((current->name)->data, name->data) != 0)
    {
        if (current->next == NULL)
        {
            return NULL;
        }
        else
        {
            current = current->next;
        }
    }
    return current;
}

// delete user macro with given name
macro_node_t *macro_node_delete(string_t *name)
{
    macro_node_t *current = head;
    macro_node_t *previous = NULL;

    if (head == NULL)
    {
        return NULL;
    }

    while (strcmp((current->name)->data, name->data) != 0)
    {
        if (current->next == NULL)
        {
            return NULL;
        }
        else
        {
            previous = current;
            current = current->next;
        }
    }

    if (current == head)
    {
        head = head->next;
    }
    else
    {
        previous->next = current->next;
    }

    return current;
}

// add new user macro to linked list chain
void macro_insert_first(string_t *name, string_t *value)
{
    if (macro_node_find(name) != NULL)
    {
        error_message(__LINE__);
    }
    macro_node_t *node = (macro_node_t *)malloc(sizeof(macro_node_t));

    node->name = name;
    node->value = value;

    node->next = head;

    head = node;
    return;
}

void macro_free_all()
{
    macro_node_t *cur = head;
    macro_node_t *next = head;

    while (cur != NULL)
    {
        next = cur->next;
        macro_node_free(cur);
        cur = next;
    }
}
// print out all user macros
void macro_print_all()
{
    macro_node_t *ptr = head;
    printf("\n {");

    while (ptr != NULL)
    {
        printf("(%s, %s)", (ptr->name)->data, (ptr->value)->data);
        ptr = ptr->next;
    }

    printf("]\n");
}

char string_pop_char(string_t *str)
{
    if (str->size > 0)
    {
        int c = str->data[(str->size) - 1];
        (str->size)--;
        str->data[str->size] = '\0';

        return c;
    }
    else
    {
        return EOF;
    }
}

// add character to end of a string_t
void string_put_char(string_t *str, char c)
{
    if ((str->size)++ >= str->capacity)
    {
        string_grow(str, (str->capacity) * 2);
    }

    char char_to_str[2];

    if (c == 92)
    {
        char_to_str[0] = 92;
    }
    else
    {
        char_to_str[0] = c;
    }

    char_to_str[1] = '\0';
    strcat(str->data, char_to_str);

    return;
}

char *string_reverse(char *str)
{
    char *p1, *p2;

    if (!str || !*str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}
// increase size of dynamic array for string_t
void string_grow(string_t *str, size_t new_capacity)
{
    str->data = (char *)realloc(str->data, new_capacity + 1);
    str->capacity = new_capacity;
    return;
}

// expands a user macro with arg back into input
void user_macro_expand(string_t *str, string_t *input, string_t *arg)
{
    string_t *tmp = string_malloc(16);
    int current = 0;

    for (size_t i = 0; i < str->size; i++)
    {
        current = (str->data)[i];
        if (current == '#')
        {
            string_expand(arg, tmp);
        }
        else if (current == '\\')
        {
            if ((str->data)[i + 1] == '#')
            {
                string_put_char(tmp, '#');
                i++;
            }
            else if ((str->data)[i + 1] == '\\')
            {
                string_put_char(tmp, '\\');
                string_put_char(tmp, '\\');

                i++;
            }
            else
            {
                string_put_char(tmp, '\\');
            }
        }
        else
        {
            string_put_char(tmp, current);
        }
    }

    string_unget(tmp, input);

    string_free(tmp);
    return;
}

// appends str to dest
void string_expand(string_t *str, string_t *input)
{
    for (size_t i = 0; i < str->size; i++)
    {
        string_put_char(input, (str->data)[i]);
    }
    return;
}

// exit if string contains non alnum character
void string_isalnum(string_t *str)
{
    for (size_t i = 0; i < str->size - 1; i++)
    {
        if (!isalnum((str->data)[i]))
        {
            error_message(__LINE__);
        }
    }
}

void string_to_file(string_t *str, FILE *dst_file)
{
    for (size_t i = 0; i < str->size; i++)
    {
        fputc((str->data)[i], dst_file);
    }

    return;
}

// transfer src into dst back->front
void file_unget(char *filename, string_t *input)
{
    FILE *to_unget = fopen(filename, "r");

    int ft;
    int i = 0;

    fseek(to_unget, 0, SEEK_END);

    ft = ftell(to_unget);

    while (i < ft)
    {
        i++;
        fseek(to_unget, -i, SEEK_END);

        string_put_char(input, fgetc(to_unget));
    }
    fclose(to_unget);
}
// initialize new string_t
string_t *string_malloc(size_t capacity)
{
    string_t *str = (string_t *)malloc(sizeof(string_t));
    str->capacity = capacity;
    str->size = 0;
    str->data = (char *)malloc(capacity * sizeof(char) + 1);
    str->data = strcpy(str->data, "");
    return str;
}

// free string_t
void string_free(string_t *str)
{
    free(str->data);
    free(str);
    return;
}
// if arguments are provided, treats them as files and combines all files into a single file named "combined_inputs.txt"
// if no arguments are provided, reads stdin one character at a time and writes it into "combined_inputs"
void combine_files(int argc, char const *argv[])
{
    if (argc == 1)
    {
        remove_comments(stdin);
    }
    else
    {
        FILE *current_input;

        for (size_t i = 1; i < argc; i++)
        {
            current_input = fopen(argv[i], "r");
            remove_comments(current_input);
            fclose(current_input);
        }
    }

    return;
}

//  Reads from "combined_inputs.txt" to remove all comments and output to "no_comments.txt"
void remove_comments(FILE *data_source)
{
    FILE *combined_inputs = fopen("combined_inputs.txt", "a");

    char c = 0;
    char prev = c;

    // 0 - default, print directly
    // 1 - inside comment, ignore except for newline
    // 2 - after newline, ignore blank and tab characters
    int state = 0;

    for (;;)
    {
        prev = c;
        c = getc(data_source);
        // always end reading if reach EOF
        if (c == EOF)
        {
            break;
        }
        switch (state)
        {
        // pass every character except % through
        // if % switch to reading as a comment state 1
        case 0:
            if (c == '%')
            {
                if (prev == '\\')
                {
                    putc('%', combined_inputs);
                }
                else
                {
                    state = 1;
                }
            }
            else
            {
                putc(c, combined_inputs);
            }
            break;
        // reading comment, ignore until newline
        case 1:
            if (c == '\n')
            {
                state = 2;
            }
            break;
        // finished reading comment, ignore until non space character, but be ready to enter another comment block
        case 2:
            if (!isspace(c))
            {
                if (c == '%')
                {
                    if (prev == '\\')
                    {
                        putc('\\', combined_inputs);
                        putc('%', combined_inputs);
                    }
                    state = 1;
                }
                else
                {
                    putc(c, combined_inputs);
                    state = 0;
                }
            }
            else if (c == '\n')
            {
                putc(c, combined_inputs);
                state = 0;
            }
            break;
        }
    }
    fclose(combined_inputs);
    return;
}

// print out from output.txt doing final pass of escape characters
void final_print()
{
    FILE *output_file = fopen("output.txt", "r");
    char c = 0;
    for (;;)
    {
        c = getc(output_file);
        if (c == EOF)
        {
            break;
        }
        else
        {
            if (c == '\\')
            {
                char tmpc = getc(output_file);

                if (tmpc == '\\' || tmpc == '#' || tmpc == '%' || tmpc == '{' || tmpc == '}')
                {
                    putc(tmpc, stdout);
                }
                else
                {
                    putc('\\', stdout);
                    putc(tmpc, stdout);
                }
            }
            else
            {
                putc(c, stdout);
            }
        }
    }
    fclose(output_file);
}

// unget string backwards for recursive evaluation
void string_unget(string_t *str, string_t *input)
{
    for (int i = (str->size) - 1; i >= 0; i--)
    {
        string_put_char(input, (str->data)[i]);
    }
}
// read macros arguments using brace balancing rules
string_t *read_bracketed_string(string_t *input)
{

    string_t *str = string_malloc(16);

    int bracket_counter = 1;
    int c = 0;
    // int prev = c;
    c = string_pop_char(input);

    if (c != '{')
    {
        error_message(__LINE__);
    }

    for (;;)
    {
        c = string_pop_char(input);

        if (c == EOF)
        {
            error_message(__LINE__);
        }

        if (c == '\\')
        {
            c = string_pop_char(input);
            string_put_char(str, '\\');
        }
        else if (c == '{')
        {
            bracket_counter += 1;
        }
        else if (c == '}')
        {
            bracket_counter -= 1;
        }

        if (bracket_counter == 0)
        {
            return str;
        }
        else
        {
            string_put_char(str, c);
        }
    }
}

string_t *file_to_backwards_string(char *filename)
{
    FILE *to_reverse = fopen(filename, "r");
    string_t *str = string_malloc(16);

    int ft;
    int i = 0;

    fseek(to_reverse, 0, SEEK_END);

    ft = ftell(to_reverse);

    while (i < ft)
    {
        i++;
        fseek(to_reverse, -i, SEEK_END);

        string_put_char(str, fgetc(to_reverse));
    }

    fclose(to_reverse);
    return (str);
}

void expand_string(string_t *input_string, char *output_filename)
{
    string_t *macro_name = string_malloc(16);

    FILE *output = fopen(output_filename, "w");

    State state = state_plaintext;

    int c = 0;

    // main state machine
    for (;;)
    {
        c = string_pop_char(input_string);

        switch (state)
        {
        case state_plaintext:
        {
            if (c == EOF)
            {
                goto end_main_loop;
            }
            else if (c == '\\')
            {
                state = state_escape;
            }
            else
            {
                putc(c, output);
            }
            break;
        }
        case state_escape:
        {
            if (c == EOF)
            {
                putc('\\', output);
                goto end_main_loop;
            }
            if (c == '\\' || c == '#' || c == '%' || c == '{' || c == '}')
            {
                putc('\\', output);
                putc(c, output);
                state = state_plaintext;
            }
            else if (isalnum(c))
            {
                state = state_read_macro_name;

                string_free(macro_name);
                macro_name = string_malloc(16);

                string_put_char(macro_name, c);
            }
            else
            {
                state = state_plaintext;

                fputc('\\', output);
                fputc(c, output);
            }

            break;
        }

        case state_read_macro_name:
        {
            if (c == '{')
            {
                // bad coding that this is here but whatever
                // ungetc(c, input);
                string_put_char(input_string, c);

                // BUILT IN MACROS
                if (strcmp(macro_name->data, "def") == 0)
                {
                    string_t *name;
                    string_t *value;

                    name = read_bracketed_string(input_string);
                    string_isalnum(name);
                    value = read_bracketed_string(input_string);

                    macro_insert_first(name, value);
                    state = state_plaintext;
                }
                else if (strcmp(macro_name->data, "undef") == 0)
                {
                    string_t *name;
                    name = read_bracketed_string(input_string);

                    macro_node_t *node = macro_node_delete(name);

                    if (node == NULL)
                    {
                        error_message(__LINE__);
                    }
                    else
                    {
                        macro_node_free(node);
                    }
                    string_free(name);
                    state = state_plaintext;
                }
                else if (strcmp(macro_name->data, "if") == 0)
                {
                    string_t *cond;
                    string_t *then;
                    string_t *alt;

                    cond = read_bracketed_string(input_string);
                    then = read_bracketed_string(input_string);
                    alt = read_bracketed_string(input_string);
                    if (strcmp(cond->data, "") != 0)
                    {

                        string_unget(then, input_string);
                    }
                    else
                    {
                        string_unget(alt, input_string);
                    }

                    string_free(cond);
                    string_free(then);
                    string_free(alt);
                    state = state_plaintext;
                }
                else if (strcmp(macro_name->data, "ifdef") == 0)
                {
                    string_t *name;
                    string_t *then;
                    string_t *alt;

                    name = read_bracketed_string(input_string);
                    then = read_bracketed_string(input_string);
                    alt = read_bracketed_string(input_string);

                    if (macro_node_find(name) != NULL)
                    {
                        string_unget(then, input_string);
                    }
                    else
                    {
                        string_unget(alt, input_string);
                    }

                    string_free(name);
                    string_free(then);
                    string_free(alt);
                    state = state_plaintext;
                }
                else if (strcmp(macro_name->data, "include") == 0)
                {
                    string_t *path = read_bracketed_string(input_string);
                    file_unget(path->data, input_string);
                    string_free(path);
                    state = state_plaintext;
                }
                else if (strcmp(macro_name->data, "expandafter") == 0)
                {
                    string_t *before = read_bracketed_string(input_string);
                    string_t *after = read_bracketed_string(input_string);

                    after->data = string_reverse(after->data);

                    expand_string(after, "tmp1.txt");
                    file_unget("tmp1.txt", input_string);
                    string_unget(before, input_string);

                    string_free(before);
                    string_free(after);
                    state = state_plaintext;
                }
                else
                {
                    // Check user macros
                    macro_node_t *search = macro_node_find(macro_name);
                    if (search == NULL)
                    {
                        error_message(__LINE__);
                    }
                    else
                    {
                        string_t *arg = read_bracketed_string(input_string);
                        user_macro_expand(search->value, input_string, arg);
                        string_free(arg);
                    }
                    // macro_node_free(search);
                    state = state_plaintext;
                }
            }
            else if (!isalnum(c))
            {
                error_message(__LINE__);
            }
            else
            {
                string_put_char(macro_name, c);
            }

            break;
        }
        }
    }
end_main_loop:
    fclose(output);
    string_free(macro_name);
    return;
}

void expand_file(char *input_filename, char *output_filename)
{
    // clock_t t = clock();

    string_t *input_string = file_to_backwards_string(input_filename);
    // input_string = input_combined_string

    // t = clock() - t;
    // printf(" %f \n", ((double)t) / CLOCKS_PER_SEC);

    expand_string(input_string, output_filename);

    // t = clock() - t;
    // printf(" %f \n", ((double)t) / CLOCKS_PER_SEC);

    string_free(input_string);
}

string_t *file_to_string(char *filename)
{
    FILE *file = fopen(filename, "r");
    string_t *str = string_malloc(16);
    int c = 0;
    for (;;)
    {
        c = getc(file);
        if (c == EOF)
        {
            return str;
        }
        else
        {
            string_put_char(str, c);
        }
    }
    fclose(file);
}

int main(int argc, char const *argv[])
{
    // remove("combined_inputs.txt");
    // remove("output.txt");
    // remove("tmp1.txt");

    // gathers inputs into one location whether reading from a single file, multiple files or stdin
    // removes comments during process
    // now all input is gathered in "combined_inputs.txt"
    combine_files(argc, argv);

    expand_file("combined_inputs.txt", "output.txt");

    macro_free_all();

    final_print();
    remove("combined_inputs.txt");
    remove("output.txt");
    remove("tmp1.txt");
    return 0;
}