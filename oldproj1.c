#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "proj1.h"

macro_node_t *head = NULL;
macro_node_t *current = NULL;

typedef enum
{
    state_plaintext = 0,
    state_escape = 1,
    state_read_macro_name = 2,
} State;

struct String
{
    char *data;
    size_t size;
    size_t capacity;
};

struct UserMacro
{
    string_t *name;
    string_t *value;
    macro_node_t *next;
};
struct IntStackNode{
    int data;

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

void string_put_first(string_t *str, char c)
{
    if (str->size++ >= str->capacity)
    {
        string_grow(str, str->capacity * 2);
    }

    char char_to_str[2];
    char_to_str[1] = '\0';
    char_to_str[0] = (char)c;

    strcat(char_to_str, str->data);

    return;
}

// add character to end of a string_t
void string_put_char(string_t *str, char c)
{
    if (str->size++ >= str->capacity)
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

// increase size of dynamic array for string_t
void string_grow(string_t *str, size_t new_capacity)
{
    str->data = (char *)realloc(str->data, new_capacity + 1);
    str->capacity = new_capacity;
    return;
}

// expands a user macro with arg back into input
void user_macro_expand(string_t *str, FILE *src, string_t *arg)
{
    string_t *tmp = string_malloc(16);
    int current = 0;
    int previous = 0;

    for (size_t i = 0; i < str->size; i++)
    {
        current = (str->data)[i];
        if (current == '#')
        {
            if (previous == '\\')
            {

                string_put_char(tmp, '#');
            }
            else
            {
                string_expand(arg, tmp);
            }
        }
        else if (current == '\\')
        {
            if ((str->data)[i + 1] != '#')
            {
                string_put_char(tmp, '\\');
            }
        }
        else
        {
            string_put_char(tmp, current);
        }

        previous = (str->data)[i];
    }

    string_unget(tmp, src);

    string_free(tmp);
    return;
}

// appends str to dest
void string_expand(string_t *str, string_t *dst)
{
    for (size_t i = 0; i < str->size; i++)
    {
        string_put_char(dst, (str->data)[i]);
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
void file_unget(string_t *filename, FILE *dst)
{
    FILE *to_unget = fopen(filename->data, "r");

    int ft;
    int i = 0;
    if (to_unget == NULL)
    {
        error_message(__LINE__);
    }
    else
    {
        fseek(to_unget, 0, SEEK_END);

        ft = ftell(to_unget);

        while (i < ft)
        {
            i++;
            fseek(to_unget, -i, SEEK_END);

            ungetc(fgetc(to_unget), dst);
        }
    }
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

    // The comment character, %, should cause your program to ignore it and all subsequent characters up to the first non-blank, non-tab character following the next newline or the end of the current file, whichever comes first.
    char c = 0;

    // 0 - default, print directly
    // 1 - inside comment, ignore except for newline
    // 2 - after newline, ignore blank and tab characters
    int state = 0;

    for (;;)
    {
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
                state = 1;
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
                    state = 1;
                }
                else
                {
                    putc(c, combined_inputs);
                    state = 0;
                }
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
                    putc(c, stdout);
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
void string_unget(string_t *str, FILE *src)
{
    for (int i = (str->size) - 1; i >= 0; i--)
    {
        ungetc((str->data)[i], src);
    }
}
// read macros arguments using brace balancing rules
string_t *read_bracketed_string(FILE *input)
{

    string_t *str = string_malloc(16);

    int bracket_counter = 1;
    int c = 0;
    c = getc(input);

    if (c != '{')
    {
        error_message(__LINE__);
    }

    for (;;)
    {
        c = getc(input);
        if (c == EOF)
        {
            error_message(__LINE__);
        }
        if (c == '{')
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

void expand_file(char *input_filename, char *output_filename)
{
    string_t *macro_name = string_malloc(16);
    FILE *input = fopen(input_filename, "r");
    FILE *output = fopen(output_filename, "w");
    file_to_backwards_string(input) ;

    State state = state_plaintext;
    int c = 0;

    // main state machine
    for (;;)
    {
        c = getc(input);
        // macro_print_all();
        // to be removed when I implement multiple passes

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
                // putc('\\', output);
                putc(c, output);
                state = state_plaintext;
                // error_message(__LINE__);
            }
            break;
        }

        case state_read_macro_name:
        {
            if (c == '{')
            {
                // bad coding that this is here but whatever
                ungetc(c, input);

                // BUILT IN MACROS
                if (strcmp(macro_name->data, "def") == 0)
                {
                    string_t *name;
                    string_t *value;

                    name = read_bracketed_string(input);
                    string_isalnum(name);
                    value = read_bracketed_string(input);

                    macro_insert_first(name, value);
                    state = state_plaintext;
                }
                else if (strcmp(macro_name->data, "undef") == 0)
                {
                    string_t *name;
                    name = read_bracketed_string(input);

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

                    cond = read_bracketed_string(input);
                    then = read_bracketed_string(input);
                    alt = read_bracketed_string(input);
                    if (strcmp(cond->data, "") != 0)
                    {

                        string_unget(then, input);
                    }
                    else
                    {
                        string_unget(alt, input);
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

                    name = read_bracketed_string(input);
                    then = read_bracketed_string(input);
                    alt = read_bracketed_string(input);

                    if (macro_node_find(name) != NULL)
                    {
                        string_unget(then, input);
                    }
                    else
                    {
                        string_unget(alt, input);
                    }

                    string_free(name);
                    string_free(then);
                    string_free(alt);
                    state = state_plaintext;
                }
                else if (strcmp(macro_name->data, "include") == 0)
                {
                    string_t *path = read_bracketed_string(input);
                    file_unget(path, input);
                    string_free(path);
                    state = state_plaintext;
                }
                else if (strcmp(macro_name->data, "expandafter") == 0)
                {
                    string_t *before = read_bracketed_string(input);
                    string_t *after = read_bracketed_string(input);

                    FILE *tmpfile = fopen("tmp.txt", "w");

                    string_to_file(after, tmpfile);
                    fclose(tmpfile);

                    expand_file("tmp.txt", "tmp2.txt");
                    tmpfile = fopen("tmp2.txt", "r");

                    string_t *to_unget = file_to_string(tmpfile);
                    fclose(tmpfile);

                    string_unget(to_unget, input);

                    string_unget(before, input);

                    string_free(before);
                    string_free(after);
                    string_free(to_unget);
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
                        string_t *arg = read_bracketed_string(input);
                        user_macro_expand(search->value, input, arg);
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

    fclose(input);
    fclose(output);

    // macro_print_all();

    string_free(macro_name);
    return;
}

string_t *file_to_string(FILE *file)
{
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
}

int main(int argc, char const *argv[])
{

    remove("combined_inputs.txt");
    remove("output.txt");

    // gathers inputs into one location whether reading from a single file, multiple files or stdin
    // removes comments during process
    // now all input is gathered in "combined_inputs.txt"
    combine_files(argc, argv);

    expand_file("combined_inputs.txt", "output.txt");

    macro_free_all();

    final_print();
    // remove("combined_inputs.txt");
    // remove("output.txt");
    return 0;
}