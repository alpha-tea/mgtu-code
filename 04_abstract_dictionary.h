#ifndef ABSTRACT_DICTIONARY
#define ABSTRACT_DICTIONARY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum data_types { undefined, bytes, word, floating, text};

static const char* type_names[] = {"data", "byte", "word", "float", "text"};

struct dictionary {
    unsigned int elements;
    unsigned int dict_size;
    unsigned int *sizes;
    struct key_value {
        char* key;
        void* value;
    } *data;
    enum data_types* types;
};

int create_key_value(struct key_value* pair, char* key, void* value, unsigned int val_size);
int destroy_key_value(struct key_value* pair);
int copy_key_value(struct key_value* dst, struct key_value* src, unsigned int size);
int compare_key_value(struct key_value* pair_a, struct key_value* pair_b, enum data_types type, unsigned int size);
void create_dictionary(struct dictionary* dict);
void destroy_dictionary(struct dictionary* dict);
void print_dictionary(struct dictionary* dict);
int find_in_dictionary(struct dictionary* dict, struct key_value* pair, int* idx);
int add_to_dictionary(struct dictionary* dict, struct key_value* pair, enum data_types type, unsigned int val_size);
int sub_from_dictionary(struct dictionary* dict, struct key_value* pair);

#endif
