#include "04_abstract_dictionary.h"

/*
 Лабораторная №1. 2-й семестр.
 Реализовать абстрактную структуру данных словарь (коллекция ключ - значение).
 Ключ - C - строка.
 Значение может быть любым. Для работы необходимо реализовать две структуры и функции,
 которые работают с ними.
 1. структура keyvalue для хранения ключа и значения.
 2. для работы с keyvalue реализовать функции:
 а) создание
 б) уничтожение
 в) копирование
 г) сравнение
 3. структура dictionary для реализации списка
 4. для работы с dictionary реализовать функции:
 а) создание
 б) уничтожение
 в) добавление элемента (realloc)
 г) удаление элемента по ключу
 д) поиск элемента по ключу
 е) печать (если значение имеет базовый тип)
 применить модульный подход (заголовочный, исполняемый и main файлы)
*/

int create_key_value(struct key_value* pair, char* key, void* value, unsigned int val_size)
{
    if (pair == NULL || key == NULL || value == NULL || val_size == 0) {
        printf("pair, key or value is NULL, or incorrect size;\n");
        return -1;
    }
    int size = 0;
    while (key[size] != '\0')
        ++size;
    if (pair->key != NULL || pair->value != NULL) {
        printf("warning: key is already created, maybe memory leak;\n");
        return -1;
    }
    pair->key = (char*)malloc(size + 1);
    pair->value = malloc(val_size);
    if (pair->value == NULL || pair->key == NULL) {
        printf("error memory (re)allocating for pair structure or key(%p) or value(%p);\n",
               pair->key, pair->value);
        exit(-1);
    }
    memccpy(pair->key, key, '\0', size + 1);
    memcpy(pair->value, value, val_size);
    printf("new pair created, key at %p and value at %p;\n", pair->key, pair->value);
    return 0;
}

int destroy_key_value(struct key_value* pair)
{
    if (pair == NULL || pair->key == NULL || pair->value == NULL) {
        printf("pair, key or value is NULL;\n");
        return -1;
    }
    free(pair->key);
    free(pair->value);
    pair->key = NULL;
    pair->value = NULL;
    printf("pair at %p, free key and value memory;\n", pair);
    return 0;
}

int copy_key_value(struct key_value* dst, struct key_value* src, unsigned int size)
{   // данные в памяти приёмника полностью освобождаются, максимальный размер в байтах
    if (size == 0 || dst == NULL || src == NULL || src == dst || src->key[0] == '\0') {
        printf("dst or src is NULL, or src and dts are the same, or size incorrect;\n");
        return -1;
    }
    if (dst->key != NULL)
        free(dst->key);
    if (dst->value != NULL)
        free(dst->value);
    int len = 0;
    while (src->key[len] != '\0')
        ++len;
    if ((dst->key = (char*)malloc(len + 1)) == NULL) {
        printf("error memory (re)allocating for dst structure or key;\n");
        exit(-1);
    }
    memccpy(dst->key, src->key, '\0', len + 1);
    if ((dst->value = (char*)malloc(size)) == NULL) {
        printf("error memory (re)allocating for dst structure or value;\n");
        exit(-1);
    }
    memcpy(dst->value, src->value, size);
    printf("copy pair key value, success new memory allocated at %p;\n", dst);
    return 0;
}

int compare_key_value(struct key_value* pair_a, struct key_value* pair_b, enum data_types type, unsigned int size)
{   // сравнивается только в пределах типа или по максимальному размеру, если память
    // возвращает: 0 - если величины равны, -1 - если 1-е меньше 2-го, 1 - если наоборот
    if (size == 0 || pair_a == NULL || pair_b == NULL || type < undefined || type > text
            || pair_a->key == NULL || pair_a->value == NULL || pair_b->value == NULL
            || pair_b->key == NULL) {
        printf("a or b is NULL, or size incorrect;\n");
        return -2;
    }
    int r = 0, i = 0;
    printf("Compare data in pairs at %p and %p, type '%s' and memory size %u, iterations = ",
           pair_a, pair_b, type_names[type], size);
    switch (type) {
    case undefined:
    case bytes:
    case text:
        for (i = 0; i < size && r == 0; ++i)        //or using memcmp
            if (*((char*)pair_a->value + i) > *((char*)pair_b->value + i))
                r = 1;
            else if (*((char*)pair_a->value + i) < *((char*)pair_b->value + i))
                r = -1;
        break;
    case word:
        for (i = 0; i < size / sizeof(short) && r == 0; ++i)
            if (*((short*)pair_a->value + i) > *((short*)pair_b->value + i))
                r = 1;
            else if (*((short*)pair_a->value + i) < *((short*)pair_b->value + i))
                r = -1;
        break;
    case floating:
        for (i = 0; i < size / sizeof(float) && r == 0; ++i)
            if (*((float*)pair_a->value + i) > *((float*)pair_b->value + i))
                r = 1;
            else if (*((float*)pair_a->value + i) < *((float*)pair_b->value + i))
                r = -1;
        break;
    default:
        break;
    }
    printf("%d;\n", i);
    return r;
}

void create_dictionary(struct dictionary* dict)
{
    if (dict->data != NULL || dict->sizes != NULL || dict->types != NULL) {
        printf("error: one of addresses is not NULL, may memory leak;\n");
        return;
    }
    printf("dictionary inicialization at addr %p;\n", dict);
    dict->elements = dict->dict_size = 0;
    dict->sizes = dict->types = NULL;
    dict->data = NULL;
}

void destroy_dictionary(struct dictionary* dict)
{
    if (dict->elements == 0 || dict->data == NULL || dict->sizes == NULL || dict->types == NULL) {
        printf("dictionary is already empty or some pointers is NULL;\n");
        return;
    }
    printf("destroy dictionary with %d elements, pairs addresses:\n", dict->elements);
    while (dict->elements) {
        destroy_key_value(dict->data + dict->elements);
        --dict->elements;
    }
    free(dict->data);
    free(dict->sizes);
    free(dict->types);
    dict->data = NULL;
    dict->sizes = dict->types = NULL;
}

void print_dictionary(struct dictionary* dict)
{
    printf("\ndictionary elements %u, memory size %u;\n", dict->elements, dict->dict_size);
    printf("Addr:\t\tKey:\tType:\tSize:\tValue and memory dump:\n");
    for (int i = 0; i < dict->elements; ++i) {
        struct key_value* pair = dict->data + i;
        printf("%p\t'%s'\t'%s'\t%u\t", (void*)pair, pair->key,
               type_names[dict->types[i]], dict->sizes[i]);
        switch(dict->types[i]) {
        case undefined:
            break;
        case bytes:
            printf("'%c' ", *(char*)pair->value);
            break;
        case word:
            printf("'%d' ", *(short*)pair->value);
            break;
        case floating:
            for (int j = 0; j < dict->sizes[i] / sizeof(float); ++j)
                printf("'%.2f' ", ((float*)pair->value)[j]);
            break;
        case text:
            printf("'%s' ", (char*)pair->value);
            break;
        default:
            printf("warning: type is unknown;\n");
        }
        for (int j = 0; j < dict->sizes[i]; ++j)
            printf("0x%X ", *(unsigned char*)pair->value + j);
        printf("\n");
    }
    printf("\n");
}

int find_in_dictionary(struct dictionary* dict, struct key_value* pair, int* idx)
{
    if (dict == NULL || pair == NULL || idx == NULL) {
        printf("some pointers is NULL;\n");
        return -1;
    }
    int i = 0, j = 0, is_equal = 0;
    while (i < dict->elements && !is_equal) {
        struct key_value* pair_ptr = dict->data + i;
        printf("current key %p;\n", pair_ptr);
        for (j = 0; pair_ptr->key[j] != '\0' && pair->key[j] != '\0'; ++j)
            if (pair_ptr->key[j] != pair->key[j])
                break;
        if (pair_ptr->key[j] == pair->key[j]) {
            printf("key is founded at %p addresses, index %d;\n", pair_ptr, i);
            is_equal = 1;
        } else
            ++i;
    }
    if (is_equal) {
        *idx = i;
        return is_equal;
    } else {
        printf("key is not founded at any address in dictionary;\n");
        return 0;
    }
}

int add_to_dictionary(struct dictionary* dict, struct key_value* pair, enum data_types type, unsigned int val_size)
{
    if (dict == NULL || pair == NULL) {
        printf("some pointers is NULL;\n");
        return -1;
    }
    int index, is_equal = find_in_dictionary(dict, pair, &index);
    if (!is_equal) {
        printf("key '%s' is new or dictionary is empty;\n", pair->key);
        if (!dict->elements) {
            dict->data = (struct key_value*)calloc(1, sizeof(struct key_value));
            dict->sizes = (unsigned int*)calloc(1, sizeof(unsigned int));
            dict->types = (enum data_types*)calloc(1, sizeof(enum data_types));
        } else {
            dict->data = (struct key_value*)realloc(dict->data,
                                                    (dict->elements + 1) * sizeof(struct key_value));
            dict->sizes = (unsigned int*)realloc(dict->sizes,
                                                 (dict->elements + 1) * sizeof(unsigned int));
            dict->types = (enum data_types*)realloc(dict->types,
                                                    (dict->elements + 1) * sizeof(enum data_types));
        }
        if (dict->data == NULL || dict->sizes == NULL || dict->types == NULL) {
            printf("error: dict %p, sizes %p or types %p is NULL;\n",
                   dict->data, dict->sizes, dict->types);
            exit(-1);
        } else
            printf("memory reallocating successfully pairs %p sizes %p types %p;\n",
                   dict->data, dict->sizes, dict->types);
        struct key_value* new_pair = dict->data + dict->elements;
        printf("new: %p;\n", new_pair);
        new_pair->key = NULL;
        new_pair->value = NULL;
        if (create_key_value(new_pair, pair->key, pair->value, val_size) == -1) {
            printf("warning: can't create new pair, free new structure;\n");
            //--dict->elements;
            dict->data = (struct key_value*)realloc(dict->data, dict->elements);
            dict->sizes = (unsigned int*)realloc(dict->sizes, dict->elements);
            dict->types = (enum data_types*)realloc(dict->types, dict->elements);
            return -1;
        }
        printf("new pair added, key %s, type %s, size new bytes %u;\n",
               new_pair->key, type_names[type], val_size);
        dict->sizes[dict->elements] = val_size;
        dict->types[dict->elements] = type;
        dict->dict_size += val_size;
        dict->elements++;
    } else
        printf("the key '%s' is already in dictionary;\n", pair->key);
    return 0;
}

int sub_from_dictionary(struct dictionary* dict, struct key_value* pair)
{
    if (dict == NULL || pair == NULL) {
        printf("some pointers is NULL;\n");
        return -1;
    }
    int idx = 0;
    if (find_in_dictionary(dict, pair, &idx) == 1) {
        printf("key '%s' is founded in dictionary at index %d, clear pair;\n", pair->key, idx);
        destroy_key_value(&dict->data[idx]);
        dict->dict_size -= dict->sizes[idx];
        if (idx < dict->elements - 1) {
            printf("copy last element with key '%s' of dictionary to removed at %p;\n",
                   dict->data[dict->elements - 1].key, &dict->data[idx]);
            copy_key_value(&dict->data[idx],
                           &dict->data[dict->elements - 1], dict->sizes[dict->elements - 1]);
            dict->sizes[idx] = dict->sizes[dict->elements - 1];
            dict->types[idx] = dict->types[dict->elements - 1];
            //print_dictionary(dict);
        } else
            printf("just clear last pair with key '%s', nothing to copy;\n",
                   dict->data[dict->elements - 1].key);
        --dict->elements;
        if (dict->elements) {
            destroy_key_value(&dict->data[dict->elements]);
            dict->data = (struct key_value*)realloc(dict->data, dict->elements * sizeof(struct key_value));
            dict->sizes = (unsigned int*)realloc(dict->sizes, dict->elements * sizeof(unsigned int));
            dict->types = (enum data_types*)realloc(dict->types, dict->elements * sizeof(enum data_types));
            //print_dictionary(dict);
            if (dict->data == NULL || dict->sizes == NULL || dict->types == NULL) {
                printf("error: dict %p, sizes %p or types %p is NULL;\n",
                       dict->data, dict->sizes, dict->types);
                exit(-1);
            } else
                printf("data reallocating successfuly at %p;\n", dict->data);
        } else {
            printf("pair was last in dictionary, clear all data and free memory;\n");
            destroy_dictionary(dict);
        }
    } else {
        printf("warning: key '%s' not founded or dictionary is empty;\n", pair->key);
        return -1;
    }
    return 0;
}

void abstract_dictionary_tests(void)
 {
     printf("Simple dictionary with C-string data as key_value, using dynamic memory;\n");
     struct key_value pair_a = {NULL, NULL}, pair_b = {NULL, NULL};  // Написать комментарии и вывод printf что делается
     const char data_char = 'a';
     const short data_word = 0x1000;
     const float fl_a = 1.24, fl_b = -1.25;
     const float array[] = {1.234, 4.321};
     const char data_text_a[] = "abc", data_text_b[] = "abc";

     create_key_value(&pair_a, "A", (void*)&fl_a, sizeof(fl_a));
     create_key_value(&pair_b, "B", (void*)&fl_b, sizeof(fl_b));

     printf("key: '%s' value %.2f;\n", pair_a.key, *((float*)pair_a.value));
     printf("key: '%s' value %.2f;\n\n", pair_b.key, *((float*)pair_b.value));

     int r = compare_key_value(&pair_a, &pair_b, floating, sizeof(fl_a));
     printf("compare: %d;\n", r);

     copy_key_value(&pair_b, &pair_a, sizeof(fl_a));
     printf("key: '%s' value %.2f;\n", pair_a.key, *((float*)pair_a.value));
     printf("key: '%s' value %.2f;\n\n", pair_b.key, *((float*)pair_b.value));

     destroy_key_value(&pair_a);
     destroy_key_value(&pair_b);

     struct dictionary dict = {0, 0, NULL, NULL, NULL};
     create_dictionary(&dict);

     create_key_value(&pair_a, "A", (void*)&data_char, 1);
     add_to_dictionary(&dict, &pair_a, bytes, sizeof(char));
     add_to_dictionary(&dict, &pair_a, bytes, sizeof(char));
     print_dictionary(&dict);
     destroy_key_value(&pair_a);

     create_key_value(&pair_a, "B", (void*)&data_word, sizeof(short));
     add_to_dictionary(&dict, &pair_a, word, sizeof(short));
     print_dictionary(&dict);
     find_in_dictionary(&dict, &pair_a, &r);
     destroy_key_value(&pair_a);

     create_key_value(&pair_a, "C", (void*)data_text_a, sizeof(data_text_a));
     add_to_dictionary(&dict, &pair_a, text, sizeof(data_text_a));
     print_dictionary(&dict);

     sub_from_dictionary(&dict, &pair_a);
     print_dictionary(&dict);
     destroy_key_value(&pair_a);

     create_key_value(&pair_a, "A", (void*)&data_char, 1);
     sub_from_dictionary(&dict, &pair_a);
     print_dictionary(&dict);
     destroy_key_value(&pair_a);

     create_key_value(&pair_a, "C", (void*)data_text_a, sizeof(data_text_a));
     add_to_dictionary(&dict, &pair_a, text, sizeof(data_text_a));
     print_dictionary(&dict);
     destroy_key_value(&pair_a);

     create_key_value(&pair_a, "A", (void*)array, sizeof(array));
     add_to_dictionary(&dict, &pair_a, floating, sizeof(array));
     print_dictionary(&dict);
     destroy_key_value(&pair_a);

     destroy_dictionary(&dict);  // где-то разрушение кучи
 }
