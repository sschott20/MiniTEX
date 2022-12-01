#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct String string_t;
typedef struct IntStackNode int_node_t;
typedef struct UserMacro macro_node_t;

void int_node_push(int value, string_t *input);

int int_node_pop(string_t *input);
void int_node_push(int value, string_t *input);

string_t *file_to_string(char *filename);
string_t *file_to_backwards_string(char *filename);

void error_message();
void string_to_file(string_t *str, FILE *dst_file);
void macro_insert_first(string_t *name, string_t *value);

void macro_node_free(macro_node_t *node);

macro_node_t *macro_node_delete(string_t *name);
void user_macro_expand(string_t *str, string_t *input, string_t *arg);

void macro_print_all();

macro_node_t *macro_node_find(string_t *name);

string_t *read_bracketed_string(string_t *input);
void string_isalnum(string_t *str);

void combine_files(int argc, char const *argv[]);

void remove_comments();

void final_print();

void string_expand(string_t *str, string_t *dst);

void string_put_char(string_t *str, char c);

void string_unget(string_t *str, string_t *input);

void string_grow(string_t *str, size_t new_capacity);

string_t *string_malloc(size_t capacity);

char string_pop_first(string_t *str);

void string_free(string_t *str);
char string_pop_char(string_t *str);

void file_unget(char *filename, string_t *input);

void macro_free_all();

void expand_file(char *input_filename, char *output_filename);
