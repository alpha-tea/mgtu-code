#ifndef BINARY_TREE
#define BINARY_TREE

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MEM_MAX 0x8000
#define IDX_MAX 0x7FFF
#define OBJ_MAX 0x0100

// Встроенные типы объектов для удобного вывода и тестирования. Неопределённые данные по умолчанию.

enum obj_type {obj_data = 0, obj_text = 1, obj_word = 2, obj_float = 3, obj_not = 4};
// Тип обхода узлов при поиске элемента для различных типов обходов дерева.
enum search_type {sr_node_left_right = 0, sr_left_node_right = 1, sr_left_right_node = 2, sr_not = 3};
// Код возвращаемой ошибки
enum errors_type {err_ok = 0, err_incorrect = 1, err_memory = 2, err_already = 3, err_not = 4};

struct binary_tree {
    unsigned short data_size;
    unsigned short memory_size; // Доп. параметр
    unsigned char obj_type;
    unsigned char obj_size;
    struct node {
        void* obj;
        struct node* previous, *left, *right;
    } *root;
};

// Глобальные константы для всей программы.
static const float float_epsilon = 0.01;
// Размеры поддерживаемых типов данных, 0 - размер неопределён
static const unsigned char obj_type_size[] = {sizeof(unsigned char), sizeof(char), sizeof(short), sizeof(float), 0};
// Имена поддерживаемых типов данных.
static const char* obj_type_name[] = {"data", "text", "word", "float", "not an object"};
// Имена поддерживаемых типов поиска.
static const char* sreach_name[] = {"node-left-right or 'pre-order'",
                                    "left-node-right or 'in-order'",
                                    "left-right-node or 'post-order'",
                                    "not a search"};
//
static const short rnd_word_max = 10;
//
static const char rnd_char_max = 'Z' - 'A';
//
static const char hex_tab[] = "0123456789ABCDEF";
//
static const char* errors[] = {"No errors.", "Incorrect parameters.", "Not enough memory.", "Node already in tree."};
//
static const char print_default_char = '.';

// Глобальные данные для всей программы.
static unsigned short memory_used = 0x0000;
static unsigned short memory_free = MEM_MAX;
static enum search_type default_search = sr_node_left_right;
static struct node* tree_nodes[MEM_MAX];
static short tree_nodes_size = 0;
static unsigned char scr_width = 128;

short is_node_correct(struct node* n);
short node_create(struct node** dst, void* data, unsigned char size);
short node_copy(struct node** dst, struct node* src, unsigned char size);
short node_move(struct node** dst, struct node** src);
short node_destroy(struct node** dst, unsigned char size);
short node_to_text(struct binary_tree* tree, struct node* leaf, char* text, short is_clear);
short nodes_compare(struct binary_tree* tree, struct node* left, struct node* right);
struct node* tree_search_node(struct binary_tree* tree, struct node* obj);
struct node* traverse_node(struct binary_tree* tree, struct node* stack[],
                           short* sp, struct node* left, struct node* right);
struct node* traverse_pop(struct binary_tree* tree, struct node* stack[], short* sp);
short tree_traverse(struct binary_tree* tree, enum search_type type);
short tree_print(struct binary_tree* tree, unsigned char endl);
short tree_add_node(struct binary_tree* tree, void* data);
void tree_replace_node(struct binary_tree* tree, struct node* dst, struct node* src);
short tree_sub_node(struct binary_tree* tree, void* data);
short tree_create(struct binary_tree* tree, void* data, short objects, unsigned char obj_size,
                  enum obj_type type, short is_rnd);
short tree_copy(struct binary_tree* dst, struct binary_tree* src);
short tree_move(struct binary_tree** dst, struct binary_tree** src);
short tree_destroy(struct binary_tree* tree);
short tree_compare(struct binary_tree* left, struct binary_tree* right, short is_order);
short tree_balance(struct binary_tree* dst, struct binary_tree* src);
int binary_search_tree();

#endif
