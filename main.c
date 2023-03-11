#include "04_abstract_dictionary.h"

void abstract_dictionary_tests(void)
{
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
    add_to_dictionary(&dict, &pair_a, byte, sizeof(char));
    add_to_dictionary(&dict, &pair_a, byte, sizeof(char));
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

int main()
{
    abstract_dictionary_tests();
    return 0;
}






